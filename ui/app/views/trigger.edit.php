<?php
/*
** Zabbix
** Copyright (C) 2001-2023 Zabbix SIA
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


/**
 * @var CView $this
 * @var array $data
 */

$trigger_form = (new CForm())
	->addItem((new CVar(CCsrfTokenHelper::CSRF_TOKEN_NAME, CCsrfTokenHelper::get('trigger')))->removeId())
	->setid('trigger-form')
	->setName('trigger_edit_form')
	->setAttribute('aria-labelledby', CHtmlPage::PAGE_TITLE_ID)
	->addVar('hostid', $data['hostid'])
	->addVar('context', $data['context'])
	->addVar('expression_full', $data['expression_full'], 'expression-full')
	->addVar('recovery_expression_full', $data['recovery_expression_full'], 'recovery-expression-full')
	->addVar('triggerid', $data['triggerid']);

// Enable form submitting on Enter.
$trigger_form->addItem((new CSubmitButton())->addClass(ZBX_STYLE_FORM_SUBMIT_HIDDEN));

$discovered_trigger = ($data['flags'] == ZBX_FLAG_DISCOVERY_CREATED);
$readonly = ($data['limited'] || $discovered_trigger);

if ($readonly) {
	$trigger_form
		->addItem((new CVar('opdata', $data['opdata']))->removeId())
		->addItem((new CVar('recovery_mode', $data['recovery_mode']))->removeId())
		->addItem((new CVar('type', $data['type']))->removeId())
		->addItem((new CVar('correlation_mode', $data['correlation_mode']))->removeId())
		->addItem((new CVar('manual_close', $data['manual_close']))->removeId());
}

$triggers_tab = (new CTabView())
	->addTab('triggersTab', _('Trigger'),
		new CPartial('trigger.edit.trigger.tab', $data + [
			'discovered_trigger' => $discovered_trigger,
			'readonly' => $readonly,
			'form_name' => $trigger_form->getName()
		])
	)
	->addTab('tags-tab', _('Tags'),
		new CPartial('configuration.tags.tab', [
			'source' => 'trigger',
			'tags' => $data['tags'],
			'show_inherited_tags' => $data['show_inherited_tags'],
			'readonly' => $discovered_trigger,
			'tabs_id' => 'tabs',
			'tags_tab_id' => 'tags-tab',
			'field_label' => _('Tags')
		]),
		TAB_INDICATOR_TAGS
	)
	->addTab('dependenciesTab', _('Dependencies'),
		new CPartial('trigger.edit.dependencies.tab', $data + [
			'discovered_trigger' => $discovered_trigger
		]),
		TAB_INDICATOR_DEPENDENCY);

if ($data['form_refresh'] == 0) {
	$triggers_tab->setSelected(0);
}

if (!$data['triggerid']) {
	$buttons = [
		[
			'title' => _('Add'),
			'keepOpen' => true,
			'isSubmit' => true,
			'action' => 'trigger_edit_popup.submit();'
		]
	];
}
else {
	$buttons = [
		[
			'title' => _('Update'),
			'keepOpen' => true,
			'isSubmit' => true,
			'action' => 'trigger_edit_popup.submit();'
		],
		[
			'title' => _('Clone'),
			'class' => ZBX_STYLE_BTN_ALT,
			'keepOpen' => true,
			'isSubmit' => false,
			'action' => 'trigger_edit_popup.clone();'
		],
		[
			'title' => _('Delete'),
			'confirmation' => _('Delete trigger?'),
			'class' => ZBX_STYLE_BTN_ALT,
			'keepOpen' => true,
			'isSubmit' => false,
			'enabled' => !$data['limited'],
			'action' => 'trigger_edit_popup.delete();'
		]
	];
}

$popup_parameters = [
	'dstfrm' => $data['form_name'],
	'context' => $data['context']
];

if ($data['hostid']) {
	$popup_parameters['hostid'] = $data['hostid'];
}

// Append tabs to form.
$trigger_form
	->addItem($triggers_tab)
	->addItem((new CScriptTag('trigger_edit_popup.init('.json_encode([
			'triggerid' => $data['triggerid'],
			'expression_popup_parameters' => $popup_parameters + ['dstfld1' => 'expression'],
			'recovery_popup_parameters' => $popup_parameters + ['dstfld1' => 'recovery_expression'],
			'readonly' => $readonly,
			'db_dependencies' => $data['db_dependencies'],
			'action' => 'trigger.edit',
			'context' => $data['context'],
			'db_trigger' => $data['db_trigger']
		]).');'))->setOnDocumentReady()
	);

$output = [
	'header' => $data['triggerid'] === null ? _('New trigger') : _('Trigger'),
	'doc_url' => CDocHelper::getUrl(CDocHelper::DATA_COLLECTION_TRIGGERS_EDIT),
	'body' => $trigger_form->toString(),
	'buttons' => $buttons,
	'script_inline' => getPagePostJs().$this->readJsFile('trigger.edit.js.php')
];

if ($data['user']['debug_mode'] == GROUP_DEBUG_MODE_ENABLED) {
	CProfiler::getInstance()->stop();
	$output['debug'] = CProfiler::getInstance()->make()->toString();
}

echo json_encode($output);
