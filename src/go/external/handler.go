/*
** Zabbix
** Copyright (C) 2001-2021 Zabbix SIA
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/

package external

import (
	"encoding/json"
	"fmt"
	"net"
	"os"
	"strconv"
	"time"

	"zabbix.com/pkg/plugin"
	"zabbix.com/pkg/shared"
)

const defaultTimeout = 3
const socketArg = 2
const startTypeArg = 3

const (
	Info    = 0
	Crit    = 1
	Err     = 2
	Warning = 3
	Debug   = 4
	Trace   = 5
)

type handler struct {
	name          string
	socket        string
	registerStart bool
	connection    net.Conn
}

var supportedVersion map[string]bool

func NewHandler(name string) (h handler, err error) {
	h.name = name

	if len(os.Args) < socketArg {
		panic("no socket provided")
	}

	h.socket = os.Args[1]

	if len(os.Args) < startTypeArg {
		h.registerStart = false

		return
	}

	h.registerStart, err = strconv.ParseBool(os.Args[2])
	if err != nil {
		panic(fmt.Sprintf("failed to parse third parameter %s", err.Error()))
	}

	return
}

func (h *handler) Execute() error {
	err := h.setConnection(h.socket, defaultTimeout*time.Second)
	if err != nil {
		return err
	}

	err = h.start()
	if err != nil {
		return err
	}

	h.run()

	return nil
}

func (h *handler) run() {
	for {
		err := h.handle()
		if err != nil {
			h.Errf(err.Error())
		}
	}
}

func (h *handler) handle() error {
	reqType, data, err := shared.Read(h.connection)
	if err != nil {
		return err
	}

	h.Tracef("Plugin %s executing %s", h.name, shared.GetRequestName(reqType))

	switch reqType {
	case shared.RegisterRequestType:
		err = h.register(data)
		if err != nil {
			return err
		}
	case shared.TerminateRequestType:
		h.terminate()
	case shared.ValidateRequestType:
		err = h.validate(data)
		if err != nil {
			return err
		}
	case shared.ExportRequestType:
		err = h.export(data)
		if err != nil {
			return err
		}
	case shared.ConfigureRequestType:
		err = h.configure(data)
		if err != nil {
			return err
		}
	default:
		return fmt.Errorf("unknown request recivied: %d", reqType)
	}

	h.Tracef("Plugin %s executed %s", h.name, shared.GetRequestName(reqType))

	return nil
}

func (h *handler) start() error {
	if h.registerStart {
		return nil
	}

	acc, err := plugin.GetByName(h.name)
	if err != nil {
		return err
	}

	p, ok := acc.(plugin.Runner)
	if !ok {
		return nil
	}

	p.Start()

	return nil
}

func (h *handler) stop() error {
	if h.registerStart {
		return nil
	}

	acc, err := plugin.GetByName(h.name)
	if err != nil {
		return err
	}

	p, ok := acc.(plugin.Runner)
	if !ok {
		return nil
	}

	p.Stop()

	return nil
}

func (h *handler) register(data []byte) error {
	var req shared.RegisterRequest
	err := json.Unmarshal(data, &req)
	if err != nil {
		return err
	}

	response := createEmptyRegisterResponse(req.Id)

	err = checkVersion(req.Version)
	if err != nil {
		response.Error = err.Error()

		return shared.Write(h.connection, response)
	}

	var metrics []string

	for key, metric := range plugin.Metrics {
		metrics = append(metrics, key)
		metrics = append(metrics, metric.Description)
	}

	interfaces, err := h.getInterfaces()
	if err != nil {
		return err
	}

	response.Name = h.name
	response.Metrics = metrics
	response.Interfaces = interfaces

	return shared.Write(h.connection, response)
}

func checkVersion(version string) error {
	if supportedVersion[version] {
		return nil
	}

	return fmt.Errorf("plugin does not support version %s", version)
}

func (h *handler) validate(data []byte) error {
	var req shared.ValidateRequest
	err := json.Unmarshal(data, &req)
	if err != nil {
		return err
	}

	response := createEmptyValidateResponse(req.Id)
	acc, err := plugin.GetByName(h.name)
	if err != nil {
		return err
	}

	p, ok := acc.(plugin.Configurator)
	if !ok {
		panic("plugin does not implement Configurator interface")
	}

	err = p.Validate(req.PrivateOptions)
	if err != nil {
		response.Error = err.Error()
	}

	return shared.Write(h.connection, response)
}

func (h *handler) configure(data []byte) error {
	var req shared.ConfigureRequest
	err := json.Unmarshal(data, &req)
	if err != nil {
		return err
	}

	acc, err := plugin.GetByName(h.name)
	if err != nil {
		return err
	}

	p, ok := acc.(plugin.Configurator)
	if !ok {
		panic("plugin does not implement Configurator interface")
	}

	p.Configure(req.GlobalOptions, req.PrivateOptions)
	return nil
}

func (h *handler) export(data []byte) error {
	var req shared.ExportRequest
	err := json.Unmarshal(data, &req)
	if err != nil {
		return err
	}

	acc, err := plugin.GetByName(h.name)
	if err != nil {
		return err
	}

	p, ok := acc.(plugin.Exporter)
	if !ok {
		panic("plugin does not implement Exporter interface")
	}

	response := createEmptyExportResponse(req.Id)
	response.Value, err = p.Export(req.Key, req.Params, &emptyCtx{})
	if err != nil {
		response.Error = err.Error()
	}

	return shared.Write(h.connection, response)
}

func (h *handler) terminate() {
	err := h.stop()
	if err != nil {
		h.Errf(fmt.Sprintf("failed to execute stop: %s\n", err.Error()))
	}

	os.Exit(0)
}

func (h *handler) getInterfaces() (uint32, error) {
	var interfaces uint32
	acc, err := plugin.GetByName(h.name)
	if err != nil {
		return interfaces, err
	}

	_, ok := acc.(plugin.Exporter)
	if ok {
		interfaces |= shared.Exporter
	}

	_, ok = acc.(plugin.Configurator)
	if ok {
		interfaces |= shared.Configurator
	}

	_, ok = acc.(plugin.Runner)
	if ok {
		interfaces |= shared.Runner
	}

	_, ok = acc.(plugin.Collector)
	if ok {
		interfaces |= shared.Collector
	}

	return interfaces, nil
}

func (h *handler) Tracef(format string, args ...interface{}) {
	h.sendLog(createLogRequest(Trace, fmt.Sprintf(format, args...)))
}

func (h *handler) Debugf(format string, args ...interface{}) {
	h.sendLog(createLogRequest(Debug, fmt.Sprintf(format, args...)))
}

func (h *handler) Warningf(format string, args ...interface{}) {
	h.sendLog(createLogRequest(Warning, fmt.Sprintf(format, args...)))
}

func (h *handler) Infof(format string, args ...interface{}) {
	h.sendLog(createLogRequest(Info, fmt.Sprintf(format, args...)))
}

func (h *handler) Errf(format string, args ...interface{}) {
	h.sendLog(createLogRequest(Err, fmt.Sprintf(format, args...)))
}

func (h *handler) Critf(format string, args ...interface{}) {
	h.sendLog(createLogRequest(Crit, fmt.Sprintf(format, args...)))
}

func createLogRequest(severity uint32, message string) shared.LogRequest {
	return shared.LogRequest{
		Common: shared.Common{
			Id:   shared.NonRequiredID,
			Type: shared.LogRequestType,
		},
		Severity: severity,
		Message:  message,
	}
}

func createEmptyRegisterResponse(id uint32) shared.RegisterResponse {
	return shared.RegisterResponse{
		Common: shared.Common{
			Id:   id,
			Type: shared.RegisterResponseType,
		},
	}
}

func createEmptyExportResponse(id uint32) shared.ExportResponse {
	return shared.ExportResponse{Common: shared.Common{Id: id, Type: shared.ExportResponseType}}
}

func createEmptyValidateResponse(id uint32) shared.ValidateResponse {
	return shared.ValidateResponse{Common: shared.Common{Id: id, Type: shared.ValidateResponseType}}
}

func (h *handler) sendLog(request shared.LogRequest) {
	err := shared.Write(h.connection, request)
	if err != nil {
		panic(fmt.Sprintf("failed to log message %s", err.Error()))
	}
}

func init() {
	supportedVersion = map[string]bool{}
	supportedVersion[shared.Version] = true
}
