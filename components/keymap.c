/* See LICENSE file for copyright and license details. */
#include <err.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

#if USE_X
#	include <xcb/xkb.h>
#	include "../X.h"
#endif

#include "../lib/util.h"
#include "../aslstatus.h"
#include "../components_config.h"

#ifndef KEYMAP_NUMLOCK
#	define KEYMAP_NUMLOCK ""
#endif

#if USE_X && USE_XKB
struct layout {
	char	group[3];
	uint8_t lock_mask;
};

enum lock_mask {
	CAPS = 1 << 0,
	NUM  = 1 << 1,
};

static void    init_events(xcb_connection_t *c);
static uint8_t init_xkb_extension(xcb_connection_t *);
static uint8_t get_layout_struct(xcb_connection_t *, struct layout *);

void
keymap(char *	  layout,
       const char __unused *_a,
       uint32_t __unused    _i,
       static_data_t *	    static_data)
{
	void *ev = NULL;

	struct layout state;

	uint8_t *event_initialized = static_data->data;

	if (!*event_initialized) {
		if (!init_xkb_extension(X_CONNECTION)) {
			warnx("xcb: failed to initialize xkb extension");
			ERRRET(layout);
		}
		init_events(X_CONNECTION);

		*event_initialized = !0;
	} else {
		if (!(ev = xcb_wait_for_event(X_CONNECTION))) {
			warnx("xcb: failed to get xkb event");
			ERRRET(layout);
		}
		free(ev);
	}

	if (!!get_layout_struct(X_CONNECTION, &state)) ERRRET(layout);

	if (state.lock_mask & CAPS) {
		SAFE_ASSIGN(state.group[0], toupper(state.group[0]));
		SAFE_ASSIGN(state.group[1], toupper(state.group[1]));
	}

	bprintf(layout,
		"%s%s",
		state.group,
		(state.lock_mask & NUM) ? KEYMAP_NUMLOCK : "");
}

static uint8_t
init_xkb_extension(xcb_connection_t *c)
{
	uint8_t			       supported;
	xcb_xkb_use_extension_reply_t *reply;

	reply = xcb_xkb_use_extension_reply(
	    c,
	    xcb_xkb_use_extension(c,
				  XCB_XKB_MAJOR_VERSION,
				  XCB_XKB_MINOR_VERSION),
	    NULL);

	supported = reply->supported;

	free(reply);
	return !!supported;
}

static void
init_events(xcb_connection_t *c)
{
#	define clone_field(S, F) typeof_field(S, F) F
	struct {
		clone_field(xcb_xkb_select_events_details_t, affectState);
		clone_field(xcb_xkb_select_events_details_t, stateDetails);

		clone_field(xcb_xkb_select_events_details_t,
			    affectIndicatorState);
		clone_field(xcb_xkb_select_events_details_t,
			    indicatorStateDetails);
	} details = {
		TWICE(1 << XCB_XKB_EVENT_TYPE_STATE_NOTIFY),

		TWICE(0xFFF /* all events */),
	};

	xcb_xkb_select_events(c,
			      XCB_XKB_ID_USE_CORE_KBD,
			      XCB_XKB_EVENT_TYPE_STATE_NOTIFY
				  | XCB_XKB_EVENT_TYPE_INDICATOR_STATE_NOTIFY,
			      TWICE(TWICE(0)),
			      &details);

	xcb_flush(c);
}

static inline uint8_t
valid_layout_or_variant(char *sym)
{
	size_t i;

	/* invalid symbols from xkb rules config */
	static const char *invalid[] = { "evdev", "inet", "pc", "base" };

	for (i = 0; i < LEN(invalid); i++)
		if (!strcmp(sym, invalid[i])) return 0;

	return !0;
}

static inline char *
get_layout(char *syms, uint8_t grp_num)
{
	__typeof__(grp_num) grp;

	char *layout = NULL;
	char *tok    = strtok(syms, "+:");

	for (grp = 0; tok && grp <= grp_num; tok = strtok(NULL, "+:")) {
		if (!valid_layout_or_variant(tok)
		    /* ignore :2, :3, :4 (additional layout groups) */
		    || (strlen(tok) == 1 && isdigit(*tok)))
			continue;

		layout = tok;
		grp++;
	}

	return layout;
}

static uint8_t
get_layout_struct(xcb_connection_t *c, struct layout *ret)
{
#	define test_reply(R, F, C, COOKIE, CODE, END)                        \
		do {                                                          \
			xcb_generic_error_t *_err = NULL;                     \
                                                                              \
			R			  = F((C), (COOKIE), &_err);  \
                                                                              \
			if (_err) {                                           \
				warnx("xcb: %s: error %" PRIu8,               \
				      #F,                                     \
				      _err->error_code);                      \
				CODE = !0;                                    \
				goto END;                                     \
			}                                                     \
		} while (0)

	char *	layout;
	uint8_t error = 0;

	xcb_xkb_get_names_value_list_t names;

	xcb_get_atom_name_reply_t *atom_name   = NULL;
	xcb_xkb_get_state_reply_t *state_reply = NULL;
	xcb_xkb_get_names_reply_t *names_reply = NULL;

	test_reply(state_reply,
		   xcb_xkb_get_state_reply,
		   c,
		   xcb_xkb_get_state(c, XCB_XKB_ID_USE_CORE_KBD),
		   error,
		   end);

	test_reply(names_reply,
		   xcb_xkb_get_names_reply,
		   c,
		   xcb_xkb_get_names(c,
				     XCB_XKB_ID_USE_CORE_KBD,
				     XCB_XKB_NAME_DETAIL_SYMBOLS),
		   error,
		   end);

	xcb_xkb_get_names_value_list_unpack(
	    xcb_xkb_get_names_value_list(names_reply),
	    names_reply->nTypes,
	    names_reply->indicators,
	    names_reply->virtualMods,
	    names_reply->groupNames,
	    names_reply->nKeys,
	    names_reply->nKeyAliases,
	    names_reply->nRadioGroups,
	    names_reply->which,
	    &names);

	test_reply(atom_name,
		   xcb_get_atom_name_reply,
		   c,
		   xcb_get_atom_name(c, names.symbolsName),
		   error,
		   end);

	layout =
	    get_layout(xcb_get_atom_name_name(atom_name), state_reply->group);

	if (strnlen(layout, 2) < 2) {
		warnx("%s: invalid layout length", __func__);
		error = !0;
		goto end;
	}

	ret->group[0]  = layout[0];
	ret->group[1]  = layout[1];
	ret->group[2]  = '\0';

	ret->lock_mask = 0;
	/*
	 * LM - Lock Mask
	 * MM - Mod Mask
	 */
#	define check_mask(LM, MM)                                            \
		(__typeof__(ret->lock_mask))((LM)                             \
					     * !!(state_reply->lockedMods     \
						  & (MM)))

	ret->lock_mask |= check_mask(CAPS, XCB_MOD_MASK_LOCK);
	ret->lock_mask |= check_mask(NUM, XCB_MOD_MASK_2);

end:
	free(atom_name);
	free(state_reply);
	free(names_reply);

	return error;
}
#endif /* USE_X && USE_XKB */
