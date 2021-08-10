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

#ifndef ZABBIX_AUDIT_HOST_H
#define ZABBIX_AUDIT_HOST_H

#include "common.h"
#include "audit.h"

#define PREPARE_AUDIT_SNMP_INTERFACE(resource)									\
void	zbx_audit_##resource##_update_json_add_snmp_interface(zbx_uint64_t hostid, zbx_uint64_t version,	\
		zbx_uint64_t bulk, const char *community, const char *securityname, zbx_uint64_t securitylevel,	\
		const char *authpassphrase, const char *privpassphrase, zbx_uint64_t authprotocol,		\
		zbx_uint64_t privprotocol, const char *contextname, zbx_uint64_t interfaceid);			\
void	zbx_audit_##resource##_update_json_update_snmp_interface(zbx_uint64_t hostid, zbx_uint64_t version_old,	\
		zbx_uint64_t version_new, zbx_uint64_t bulk_old,  zbx_uint64_t bulk_new,			\
		const char *community_old, const char *community_new, const char *securityname_old,		\
		const char *securityname_new, zbx_uint64_t securitylevel_old, zbx_uint64_t securitylevel_new,	\
		const char *authpassphrase_old,	const char *authpassphrase_new, const char *privpassphrase_old,	\
		const char *privpassphrase_new,	zbx_uint64_t authprotocol_old, zbx_uint64_t authprotocol_new,	\
		zbx_uint64_t privprotocol_old, zbx_uint64_t privprotocol_new, const char *contextname_old,	\
		const char *contextname_new, zbx_uint64_t interfaceid);						\

PREPARE_AUDIT_SNMP_INTERFACE(host)
PREPARE_AUDIT_SNMP_INTERFACE(host_prototype)

void	zbx_audit_host_update_json_add_proxy_hostid_and_hostname(zbx_uint64_t hostid, zbx_uint64_t proxy_hostid,
		const char *hostname);
void	zbx_audit_host_update_json_add_tls_and_psk(zbx_uint64_t hostid, int tls_connect, int tls_accept,
		const char *psk_identity, const char *psk);
void	zbx_audit_host_update_json_add_inventory_mode(zbx_uint64_t hostid, int inventory_mode);
void	zbx_audit_host_update_json_update_inventory_mode(zbx_uint64_t hostid, int inventory_mode_old,
		int inventory_mode_new);
void	zbx_audit_host_update_json_update_host_status(zbx_uint64_t hostid, int host_status_old,
		int host_status_new);

#define PREPARE_AUDIT_HOST_INTERFACE(resource, interface_resource, type1, type2)				\
void	zbx_audit_##resource##_update_json_update_interface_##interface_resource(zbx_uint64_t hostid,		\
		zbx_uint64_t interfaceid, type1 interface_resource##_old, type1 interface_resource##_new);	\

#define	PREPARE_AUDIT_HOST(resource, audit_resource_flag)							\
void	zbx_audit_##resource##_create_entry(int audit_action, zbx_uint64_t hostid, const char *name);		\
void	zbx_audit_##resource##_update_json_add_interfaces(zbx_uint64_t hostid, zbx_uint64_t interfaceid,	\
		zbx_uint64_t main_, zbx_uint64_t type, zbx_uint64_t useip, const char *ip, const char *dns,	\
		int port);											\
PREPARE_AUDIT_HOST_INTERFACE(resource, useip, zbx_uint64_t, uint64)						\
PREPARE_AUDIT_HOST_INTERFACE(resource, main, zbx_uint64_t, uint64)						\
PREPARE_AUDIT_HOST_INTERFACE(resource, type, zbx_uint64_t, uint64)						\
PREPARE_AUDIT_HOST_INTERFACE(resource, ip, const char*, string)							\
PREPARE_AUDIT_HOST_INTERFACE(resource, dns, const char*, string)						\
PREPARE_AUDIT_HOST_INTERFACE(resource, port, int, int)								\
/* snmp */													\
PREPARE_AUDIT_HOST_INTERFACE(resource, version, zbx_uint64_t, uint64)						\
PREPARE_AUDIT_HOST_INTERFACE(resource, bulk, zbx_uint64_t, uint64)						\
PREPARE_AUDIT_HOST_INTERFACE(resource, community, const char*, string)						\
PREPARE_AUDIT_HOST_INTERFACE(resource, securityname, const char*, string)					\
PREPARE_AUDIT_HOST_INTERFACE(resource, securitylevel, int, int)							\
PREPARE_AUDIT_HOST_INTERFACE(resource, authpassphrase, const char*, string)					\
PREPARE_AUDIT_HOST_INTERFACE(resource, privpassphrase, const char*, string)					\
PREPARE_AUDIT_HOST_INTERFACE(resource, authprotocol, zbx_uint64_t, uint64)					\
PREPARE_AUDIT_HOST_INTERFACE(resource, privprotocol, zbx_uint64_t, uint64)					\
PREPARE_AUDIT_HOST_INTERFACE(resource, contextname, const char*, string)					\

PREPARE_AUDIT_HOST(host, AUDIT_RESOURCE_HOST)
PREPARE_AUDIT_HOST(host_prototype, AUDIT_RESOURCE_HOST_PROTOTYPE)

#define PREPARE_AUDIT_HOST_UPDATE(resource, type1, type2)						\
void	zbx_audit_host_update_json_update_##resource(zbx_uint64_t hostid, type1 old_##resource,		\
		type1 new_##resource);									\

PREPARE_AUDIT_HOST_UPDATE(host, const char*, string)
PREPARE_AUDIT_HOST_UPDATE(name, const char*, string)
PREPARE_AUDIT_HOST_UPDATE(proxy_hostid, zbx_uint64_t, uint64)
PREPARE_AUDIT_HOST_UPDATE(ipmi_authtype, int, int)
PREPARE_AUDIT_HOST_UPDATE(ipmi_privilege, int, int)
PREPARE_AUDIT_HOST_UPDATE(ipmi_username, const char*, string)
PREPARE_AUDIT_HOST_UPDATE(ipmi_password, const char*, string)
PREPARE_AUDIT_HOST_UPDATE(tls_connect, int, int)
PREPARE_AUDIT_HOST_UPDATE(tls_accept, int, int)
PREPARE_AUDIT_HOST_UPDATE(tls_issuer, const char*, string)
PREPARE_AUDIT_HOST_UPDATE(tls_subject, const char*, string)
PREPARE_AUDIT_HOST_UPDATE(tls_psk_identity, const char*, string)
PREPARE_AUDIT_HOST_UPDATE(tls_psk, const char*, string)
PREPARE_AUDIT_HOST_UPDATE(custom_interfaces, int, int)
#undef PREPARE_AUDIT_HOST_UPDATE

void	zbx_audit_hostgroup_update_json_attach(zbx_uint64_t hostid, zbx_uint64_t hostgroupid, zbx_uint64_t groupid);
void	zbx_audit_host_hostgroup_delete(zbx_uint64_t hostid, const char* hostname, zbx_vector_uint64_t *hostgroupids,
		zbx_vector_uint64_t *groupids);
void	zbx_audit_host_del(zbx_uint64_t hostid, const char *hostname);
void	zbx_audit_host_update_json_add_details(zbx_uint64_t hostid, const char *host, zbx_uint64_t proxy_hostid,
		int ipmi_authtype, int ipmi_privilege, const char *ipmi_username, const char *ipmi_password,
		int status, int flags, int tls_connect, int tls_accept, const char *tls_issuer, const char *tls_subject,
		const char *tls_psk_identity, const char *tls_psk, int custom_interfaces);
void	zbx_audit_host_prototype_del(zbx_uint64_t hostid, const char *hostname);
void	zbx_audit_host_prototype_update_json_add_details(zbx_uint64_t hostid, zbx_uint64_t templateid, const char *name,
		int status, int discover, int custom_interfaces);
void	zbx_audit_host_prototype_update_json_add_templateid(zbx_uint64_t hostid, zbx_uint64_t templateid);

#define PREPARE_AUDIT_HOST_PROTOTYPE_UPDATE(resource, type1, type2)						\
void	zbx_audit_host_prototype_update_json_update_##resource(zbx_uint64_t hostid, type1 old_##resource,	\
		type1 new_##resource);										\

PREPARE_AUDIT_HOST_PROTOTYPE_UPDATE(name, const char*, string)
PREPARE_AUDIT_HOST_PROTOTYPE_UPDATE(status, int, int)
PREPARE_AUDIT_HOST_PROTOTYPE_UPDATE(discover, int, int)
PREPARE_AUDIT_HOST_PROTOTYPE_UPDATE(custom_interfaces, int, int)
#undef PREPARE_AUDIT_HOST_PROTOTYPE_UPDATE

void	zbx_audit_host_prototype_update_json_add_group_details(zbx_uint64_t hostid, const char* name,
		zbx_uint64_t groupid, zbx_uint64_t templateid);
void	zbx_audit_host_update_json_add_parent_template(zbx_uint64_t hostid, zbx_uint64_t templateid);
void	zbx_audit_host_prototype_update_json_add_templates(zbx_uint64_t hostid, zbx_vector_uint64_t *templateids);
#endif	/* ZABBIX_AUDIT_HOST_H */
