/************************************************************************************
**                                                                                 **
**                               mcHF QRP Transceiver                              **
**                             K Atanassov - M0NKA 2014                            **
**                              C Turner - KA7OEI 2014                             **
**                                                                                 **
**---------------------------------------------------------------------------------**
**                                                                                 **
**  File name:    	ui_menu.c                                                      **
**  Description:    main user interface configuration/adjustment menu system       **
**  Last Modified:                                                                 **
**  Licence:		CC BY-NC-SA 3.0                                                **
************************************************************************************/
// #define NEWMENU
// Common
//
#include "mchf_board.h"
#include "ui.h"
#include "ui_menu.h"
#include "ui_configuration.h"

#include <stdio.h>
#include "arm_math.h"
#include "math.h"
#include "codec.h"
//
//
//
// LCD
#include "ui_lcd_hy28.h"

// serial EEPROM driver
#include "mchf_hw_i2c2.h"

// Encoders
#include "ui_rotary.h"
//
// Codec control
#include "codec.h"
#include "softdds.h"
//
#include "audio_driver.h"
#include "audio_filter.h"
#include "audio_management.h"
#include "ui_driver.h"
//#include "usbh_usr.h"
//
#include "ui_si570.h"

#include "cat_driver.h"

// Virtual eeprom
#include "eeprom.h"
//
// CW generation
#include "cw_gen.h"
//
#include "mchf_hw_i2c2.h"


static void UiDriverUpdateMenuLines(uchar index, uchar mode, int pos);
static void UiDriverUpdateConfigMenuLines(uchar index, uchar mode);
//
//
// Public data structures
//
// ------------------------------------------------
// Transceiver state public structure
extern __IO TransceiverState 	ts;
extern __IO OscillatorState os;



// returns true if the value was changed in its value!
bool __attribute__ ((noinline)) UiDriverMenuItemChangeUInt8(int var, uint8_t mode, volatile uint8_t* val_ptr,uint8_t val_min,uint8_t val_max, uint8_t val_default, uint8_t increment) {
	uint8_t old_val = *val_ptr;

	if(var >= 1)	{	// setting increase?
		ts.menu_var_changed = 1;	// indicate that a change has occurred
		if (*val_ptr < val_max) {
			(*val_ptr)+= increment;
		}
	}
	else if(var <= -1)	{	// setting decrease?
		ts.menu_var_changed = 1;
		if (*val_ptr > val_min) {
			(*val_ptr)-= increment;
		}

	}
	if(*val_ptr < val_min) {
		ts.menu_var_changed = 1;
		*val_ptr = val_min;
	}
	if(*val_ptr > val_max) {
		ts.menu_var_changed = 1;
		*val_ptr = val_max;
	}
	if(mode == 3)	{
		ts.menu_var_changed = 1;
		*val_ptr = val_default;
	}

	return old_val != *val_ptr;
}

bool __attribute__ ((noinline)) UiDriverMenuItemChangeUInt32(int var, uint32_t mode, volatile uint32_t* val_ptr,uint32_t val_min,uint32_t val_max, uint32_t val_default, uint32_t increment) {
	uint32_t old_val = *val_ptr;
	if(var >= 1)	{	// setting increase?
		ts.menu_var_changed = 1;	// indicate that a change has occurred
		if (*val_ptr < val_max) {
			(*val_ptr)+= increment;
		}
	}
	else if(var <= -1)	{	// setting decrease?
		ts.menu_var_changed = 1;
		if (*val_ptr > val_min) {
			(*val_ptr)-= increment;
		}

	}
	if(*val_ptr < val_min) {
		ts.menu_var_changed = 1;
		*val_ptr = val_min;
	}
	if(*val_ptr > val_max) {
		ts.menu_var_changed = 1;
		*val_ptr = val_max;
	}
	if(mode == 3)	{
		ts.menu_var_changed = 1;
		*val_ptr = val_default;
	}
	return old_val != *val_ptr;
}

bool __attribute__ ((noinline)) UiDriverMenuItemChangeInt(int var, uint32_t mode, volatile int* val_ptr,int val_min,int val_max, int val_default, uint32_t increment) {
	uint32_t old_val = *val_ptr;
	if(var >= 1)	{	// setting increase?
		ts.menu_var_changed = 1;	// indicate that a change has occurred
		if (*val_ptr < val_max) {
			(*val_ptr)+= increment;
		}
	}
	else if(var <= -1)	{	// setting decrease?
		ts.menu_var_changed = 1;
		if (*val_ptr > val_min) {
			(*val_ptr)-= increment;
		}

	}
	if(*val_ptr < val_min) {
		ts.menu_var_changed = 1;
		*val_ptr = val_min;
	}
	if(*val_ptr > val_max) {
		ts.menu_var_changed = 1;
		*val_ptr = val_max;
	}
	if(mode == 3)	{
		ts.menu_var_changed = 1;
		*val_ptr = val_default;
	}
	return old_val != *val_ptr;
}
bool __attribute__ ((noinline)) UiDriverMenuItemChangeInt16(int var, uint32_t mode, volatile int16_t* val_ptr,int16_t val_min,int16_t val_max, int16_t val_default, uint16_t increment) {
	uint32_t old_val = *val_ptr;
	if(var >= 1)	{	// setting increase?
		ts.menu_var_changed = 1;	// indicate that a change has occurred
		if (*val_ptr < val_max) {
			(*val_ptr)+= increment;
		}
	}
	else if(var <= -1)	{	// setting decrease?
		ts.menu_var_changed = 1;
		if (*val_ptr > val_min) {
			(*val_ptr)-= increment;
		}

	}
	if(*val_ptr < val_min) {
		ts.menu_var_changed = 1;
		*val_ptr = val_min;
	}
	if(*val_ptr > val_max) {
		ts.menu_var_changed = 1;
		*val_ptr = val_max;
	}
	if(mode == 3)	{
		ts.menu_var_changed = 1;
		*val_ptr = val_default;
	}
	return old_val != *val_ptr;
}

bool __attribute__ ((noinline)) UiDriverMenuItemChangeOnOff(int var, uint8_t mode, volatile uint8_t* val_ptr, uint8_t val_default) {
	// we have to align the values to true and false, since sometimes other values are passed for true (use of temp_var)
	// but this does not work properly.

	*val_ptr = (*val_ptr)?1:0;

	return UiDriverMenuItemChangeUInt8(var, mode, val_ptr,
				0,
				1,
				val_default,
				1
		);

}

// always sets 1 or 0 as result, no matter what is passed as "true" value. Only 0 is recognized as false/
bool __attribute__ ((noinline)) UiDriverMenuItemChangeDisableOnOff(int var, uint8_t mode, volatile uint8_t* val_ptr, uint8_t val_default, char* options, uint32_t* clr_ptr) {
	bool res = UiDriverMenuItemChangeOnOff(var, mode, val_ptr, val_default);
	strcpy(options, *val_ptr?"OFF":" ON");
	if (*val_ptr) { *clr_ptr = Orange; }

	return res;
}

bool __attribute__ ((noinline)) UiDriverMenuItemChangeEnableOnOff(int var, uint8_t mode, volatile uint8_t* val_ptr, uint8_t val_default, char* options, uint32_t* clr_ptr) {
	bool res = UiDriverMenuItemChangeOnOff(var, mode, val_ptr, val_default);
	strcpy(options, *val_ptr?" ON":"OFF");
	if (!*val_ptr) { *clr_ptr = Orange; }

	return res;
}


void __attribute__ ((noinline)) UiDriverMenuMapColors(uint32_t color ,char* options,volatile uint32_t* clr_ptr) {
	char* clr_str;
	switch(color) {
	case SPEC_WHITE: 	*clr_ptr = White;	clr_str = " Wht"; 	break;
	case SPEC_BLUE:  	*clr_ptr = Blue; 	clr_str = " Blu"; 	break;
	case SPEC_RED: 		*clr_ptr = Red; 	clr_str = " Red"; 	break;
	case SPEC_MAGENTA: 	*clr_ptr = Magenta; 	clr_str = " Mag"; 	break;
	case SPEC_GREEN: 	*clr_ptr = Green; 	clr_str = " Grn"; 	break;
	case SPEC_CYAN: 	*clr_ptr = Cyan; 	clr_str = " Cyn"; 	break;
	case SPEC_YELLOW: 	*clr_ptr = Yellow; 	clr_str = " Yel"; 	break;
	case SPEC_BLACK: 	*clr_ptr = Grid; 	clr_str = " Blk"; 	break;
	case SPEC_ORANGE: 	*clr_ptr = Orange; 	clr_str = " Org"; 	break;
	case SPEC_GREY2: 	*clr_ptr = Grey; 	clr_str = "GRY2"; 	break;
	default: 			*clr_ptr = Grey; 	clr_str = " Gry";
	}
	if (options != NULL) {
		strcpy(options,clr_str);
	}
}
void __attribute__ ((noinline)) UiDriverMenuMapStrings(char* output, uint32_t value ,const uint32_t string_max, const char** strings) {
    strcpy(output,(value <= string_max)?strings[value]:"UNDEFINED");
}

char blankline[33] = "                                ";

// menu entry kind constants
enum {
  MENU_STOP = 0, // last entry in a menu / group
  MENU_ITEM, // standard menu entry
  MENU_GROUP, // menu group entry
  MENU_SEP, // separator line
  MENU_BLANK // blank
};


struct  MenuGroupDescriptor_s;

// items are stored in RAM
// Each menu group has to have a MenuGroupItem pointing to the descriptor
// a MenuGroupItem is used to keep track of the fold/unfold state and to link the
// Descriptors are stored in flash
typedef struct {
  const uint16_t menuId;
  const uint16_t kind; //
  const uint16_t number;
  const char id[4];
  const char* label;
} MenuDescriptor;

typedef struct {
  bool unfolded;
  uint16_t count;
  const MenuDescriptor* me;
} MenuGroupState;


typedef struct MenuGroupDescriptor_s {
  const MenuDescriptor* entries;
  MenuGroupState* state;
  const MenuDescriptor* parent;
} MenuGroupDescriptor;


typedef struct {
  const MenuDescriptor* entryItem;
} MenuDisplaySlot;

MenuDisplaySlot menu[MENUSIZE];


/*
 * How to create a new menu entry in an existig menu:
 * - Copy an existing entry of MENU_KIND and paste at the desired position
 * - Just assign a unique number to the "number" attribute
 * - Change the label, and implement handling
 *
 * How to create a menu group:
 * - Add menu group id entry to enum below
 * - Create a MenuDescriptor Array with the desired entries, make sure to have the MENU_STOP element at last position
 *   and that all elements have the enum value as first attribute (menuId)
 * - Added the menu group entry in the parent menu descriptor array.
 * - Create a MenuState element
 * - Added the menu group descriptor to the groups list at the position corresponding to the enum value
 *   using the descriptor array, the address of the MenuState and the address of the parent menu descriptor array.
 *
 *
 */

enum {
  MENU_BASE = 0,
  MENU_CONF,
  MENU_POW,
};

const MenuDescriptor baseGroup[] = {
    { MENU_BASE, MENU_ITEM, MENU_DSP_NR_STRENGTH, "010","DSP NR Strength" },
    { MENU_BASE, MENU_GROUP, 1, "CON","Configuration Menu"},
    { MENU_BASE, MENU_ITEM, MENU_300HZ_SEL, "500","300Hz Center Freq."  },
    { MENU_BASE, MENU_ITEM, MENU_500HZ_SEL, "501","500Hz Center Freq."},
    { MENU_BASE, MENU_ITEM, MENU_1K8_SEL, "504","1.8k Center Freq."},
    { MENU_BASE, MENU_ITEM, MENU_2K3_SEL, "506","2.3k Center Freq."},
    { MENU_BASE, MENU_ITEM, MENU_2K7_SEL, "508","2.7k Filter"},
    { MENU_BASE, MENU_ITEM, MENU_3K6_SEL, "512","3.6k Filter"},
    { MENU_BASE, MENU_STOP, 0, "   " , NULL }
};

const MenuDescriptor confGroup[] = {
    { MENU_CONF, MENU_ITEM, CONFIG_BEEP_ENABLE, "111","1eep Enabled"  },
    { MENU_CONF, MENU_GROUP, 2, "POW","Power Adjust" },
    { MENU_CONF, MENU_ITEM, CONFIG_BEEP_ENABLE, "111","2eep Enabled"  },
    { MENU_CONF, MENU_ITEM, CONFIG_BEEP_ENABLE, "111","3eep Enabled"  },
    { MENU_CONF, MENU_ITEM, CONFIG_BEEP_ENABLE, "111","4eep Enabled"  },
    { MENU_CONF, MENU_ITEM, CONFIG_BEEP_ENABLE, "111","5eep Enabled"  },
    { MENU_CONF, MENU_ITEM, CONFIG_BEEP_ENABLE, "111","6eep Enabled"  },
    { MENU_CONF, MENU_ITEM, CONFIG_BEEP_ENABLE, "111","7eep Enabled"  },
    { MENU_CONF, MENU_ITEM, CONFIG_BEEP_ENABLE, "111","8eep Enabled"  },
    { MENU_CONF, MENU_STOP, 0, "   " , NULL }
};

const MenuDescriptor powGroup[] = {
    { MENU_POW, MENU_ITEM, CONFIG_10M_5W_ADJUST, "111","10 5W"  },
    { MENU_POW, MENU_ITEM, CONFIG_12M_5W_ADJUST, "111","12 5W"  },
    { MENU_POW, MENU_ITEM, CONFIG_15M_5W_ADJUST, "111","15 5W"  },
    { MENU_POW, MENU_ITEM, CONFIG_17M_5W_ADJUST, "111","17 5W"  },
    { MENU_POW, MENU_ITEM, CONFIG_20M_5W_ADJUST, "111","20 5W"  },
    { MENU_POW, MENU_ITEM, CONFIG_30M_5W_ADJUST, "111","30 5W"  },
    { MENU_POW, MENU_ITEM, CONFIG_40M_5W_ADJUST, "111","40 5W"  },
    { MENU_POW, MENU_ITEM, CONFIG_80M_5W_ADJUST, "111","80 5W"  },
    { MENU_POW, MENU_STOP, 0, "   " , NULL }
};


MenuGroupState baseGroupState;
MenuGroupState confGroupState;
MenuGroupState powGroupState;

const MenuGroupDescriptor groups[3] = {
    { baseGroup, &baseGroupState, NULL},  // Group 0
    { confGroup, &confGroupState, baseGroup},  // Group 1
    { powGroup, &powGroupState, confGroup }  // Group 2

};
// actions
// show menu -> was previously displayed -> yes -> simply display all slots
//                                       -> no  -> get first menu group, get first entry, fill first slot, run get next entry until all slots are filled

// find next MenuEntry -> is menu group -> yes -> is unfolded -> yes -> get first item from menu group
//                                                            -> no  -> treat as normal menu entry
//                                      -> is there one more entry in my menu group ? -> yes -> takes this one
//                                                                                      -> no  -> go one level up -> get next entry in this group
//                     -> there is no such entry -> fill with dummy entry

// find prev MenuEntry -> there is prev entry in menu group -> yes -> is this unfolded menu group -> yes -> get last entry of this menu group
//                                                                                                -> no  -> fill slot with entry
//                                                          -> no  -> go one level up -> get prev entry of this level

// unfold menu group -> mark as unfold, run get next entry until all menu display slots are filled
// fold menu group   -> mark as fold,   run get next entry until all menu display slots are filled

// move to next/previous page -> (this is n times prev/next)

const MenuGroupDescriptor* UiMenu_GetGroupForEntry(const MenuDescriptor* me) {
    return me==NULL?NULL:&groups[me->menuId];
}

inline bool UiMenu_IsGroup(const MenuDescriptor *entry) {
  return entry==NULL?false:entry->kind == MENU_GROUP;
}
inline bool UiMenu_IsEntry(const MenuDescriptor *entry) {
  return entry==NULL?false:entry->kind == MENU_ITEM;
}
inline bool UiMenu_SlotIsEmpty(MenuDisplaySlot* slot) {
  return slot==NULL?false:slot->entryItem == NULL;
}
inline bool UiMenu_GroupIsUnfolded(const MenuDescriptor *group) {
  return group==NULL?false:groups[group->number].state->unfolded;
}
inline uint16_t UiMenu_MenuGroupMemberCount(const MenuGroupDescriptor* gd) {
  uint16_t retval = 0;
  if (gd != NULL) {
  if (gd->state->count == 0) {
    const MenuDescriptor* entry;
    for (entry = &gd->entries[0];entry->kind != MENU_STOP; entry++) {
      gd->state->count++;
    }
  }
  retval = gd->state->count;
  }
  return retval;
}

inline void UiMenu_GroupFold(const MenuDescriptor* entry, bool fold) {
  if (UiMenu_IsGroup(entry)) {
    groups[entry->number].state->unfolded = fold == false;
  }
}

inline const MenuDescriptor* UiMenu_GroupGetLast(const MenuGroupDescriptor *group) {
  const MenuDescriptor* retval = NULL;
  uint16_t count = UiMenu_MenuGroupMemberCount(group);
  if (count>0) {
    retval = &(group->entries[count-1]);
  }
  return retval;
}

inline const MenuDescriptor* UiMenu_GroupGetFirst(const MenuGroupDescriptor *group) {
  const MenuDescriptor* retval = NULL;
  uint16_t count = UiMenu_MenuGroupMemberCount(group);
  if (count>0) {
    retval = &(group->entries[0]);
  }
  return retval;
}

const MenuDescriptor* UiMenu_GetNextEntryInGroup(const MenuDescriptor* me) {
  const MenuDescriptor* retval = NULL;

  if (me != NULL) {
    const MenuGroupDescriptor* group_ptr = UiMenu_GetGroupForEntry(me);

    int index = (me - &group_ptr->entries[0])/sizeof(me);
    if (index < group_ptr->state->count-1) {
      retval = me + 1;
    }
  }
  return retval;
}

const MenuDescriptor* UiMenu_GetPrevEntryInGroup(const MenuDescriptor* me) {
  const MenuGroupDescriptor* group_ptr = UiMenu_GetGroupForEntry(me);
  const MenuDescriptor* retval = NULL;
  if (me != NULL && me != &group_ptr->entries[0]) {
    retval = me - 1;
  }
  return retval;
}



const MenuDescriptor* UiMenu_GetParentForEntry(const MenuDescriptor* me) {
  const MenuDescriptor* retval = NULL;
  if (me != NULL) {
    const MenuGroupDescriptor* gd = UiMenu_GetGroupForEntry(me);
    if (gd->parent != NULL) {
      if (gd->state->me == NULL ) {
        const MenuGroupDescriptor* gdp = &groups[gd->parent->number];
        uint16_t count = UiMenu_MenuGroupMemberCount(gdp);
        uint16_t idx;
        for(idx = 0; idx < count; idx++) {
          if (gdp->entries[idx].number == me->menuId) {
            gd->state->me = &gdp->entries[idx];
            break;
          }
        }
      }
      retval = gd->state->me;
    }
  }
  return retval;
}

const MenuDescriptor* UiMenu_FindNextEntryInUpperLevel(const MenuDescriptor* here) {
  const MenuDescriptor* next = NULL, *focus = here;
  if (here != NULL) {
    while(focus != NULL && next == NULL) {
      // we have a parent group, we are member of a sub menu group,
      // we need next entry in containing menu group, no matter if our menu group is folded or not
      next = UiMenu_GetNextEntryInGroup(UiMenu_GetParentForEntry(focus));
      if (next == NULL) {
        focus = UiMenu_GetParentForEntry(focus);
      }
    }
  }
  return next;
}

inline bool UiMenu_IsLastInMenuGroup(const MenuDescriptor* here) {
  const MenuGroupDescriptor* gd = UiMenu_GetGroupForEntry(here);
  return UiMenu_GroupGetLast(gd) == here;
}
inline bool UiMenu_IsFirstInMenuGroup(const MenuDescriptor* here) {
  const MenuGroupDescriptor* gd = UiMenu_GetGroupForEntry(here);
  return UiMenu_GroupGetFirst(gd) == here;
}

const MenuDescriptor* UiMenu_NextMenuEntry(const MenuDescriptor* here) {
  const MenuDescriptor* next = NULL;

  if (here != NULL) {
    if (UiMenu_IsGroup(here)) {
      // is group entry

      if (UiMenu_GroupIsUnfolded(here)) {
        const MenuGroupDescriptor* group = &groups[here->number];
        next = UiMenu_GroupGetFirst(group);
        if (next == NULL) {
          // this is an empty menu group, should not happen, does make  sense
          // but we handle this anyway
          next = UiMenu_FindNextEntryInUpperLevel(here);
        }
      } else {
        // folded group, so we behave  like a normal entry
        next = UiMenu_GetNextEntryInGroup(here);
      }
    }
    if (next == NULL) {
      // we are currently at a normal entry or a folded group or empty group (in this case these are treated as simple entries)
      // only 3 cases possible:
      //   - final entry of menu, fill next slot with blank entry, return false
      //   - next entry is normal entry (group or entry, no difference), just use this one
      //   - last entry in menu group, go up, and search for next entry in this parent menu (recursively).

      if (UiMenu_IsLastInMenuGroup(here)) {
        // we need the parent menu in order to ask for the  entry after our
        // menu group entry
        // if we cannot find the parent group, we  are top level and the last menu entry
        // so there is no further entry
        next = UiMenu_FindNextEntryInUpperLevel(here);
      } else {
        next =  UiMenu_GetNextEntryInGroup(here);
      }
    }
  }
  return next;
}


const MenuDescriptor* UiMenu_FindLastEntryInLowerLevel(const MenuDescriptor* here) {
  const MenuDescriptor *last = here;
  while (UiMenu_IsGroup(here) && UiMenu_GroupIsUnfolded(here) && here == last) {
    const MenuDescriptor* last = UiMenu_GroupGetLast(UiMenu_GetGroupForEntry(here));
    if (last) { here = last; }
  }
  return here;
}

const MenuDescriptor* UiMenu_PrevMenuEntry(const MenuDescriptor* here) {
  const MenuDescriptor* prev = NULL;


  if (here != NULL) {
    if (UiMenu_IsFirstInMenuGroup(here)) {
      // we go up, get previous entry
      // if first entry,  go one further level up, ...
      //  if not first entry -> get prev entry
      //     if normal entry or folded menu -> we are done
      //     if unfolded menu_entry -> go to last entry
      //           -> if normal entry or folded menu -> we are done
      //           -> if unfolded menu entry -> go to last entry
      prev = UiMenu_GetParentForEntry(here);
    }
    else {
      prev = UiMenu_GetPrevEntryInGroup(here);
      if (UiMenu_IsGroup(prev) && UiMenu_GroupIsUnfolded(prev)) {
        prev = UiMenu_FindLastEntryInLowerLevel(here);
      }
    }
  }
  return prev;
}



bool UiMenu_FillSlotWithEntry(MenuDisplaySlot* here, const MenuDescriptor* entry) {
  bool retval = false;
  if (entry != NULL) {
    here->entryItem = entry;
    retval = true;
  } else {
    here->entryItem = NULL;
  }
  return retval;
}


/*
 * Render a menu entry on a given menu position
 */
void UiMenu_UpdateMenuEntry(const MenuDescriptor* entry, uchar mode, uint8_t pos)
{
  uint32_t  m_clr;
  m_clr = Yellow;
  char out[40];
  const char* blank = "                               ";

  if (entry != NULL && (entry->kind == MENU_ITEM || entry->kind == MENU_GROUP)) {
    uint16_t labellen = strlen(entry->id)+strlen(entry->label) + 1;
    snprintf(out,34,"%s-%s%s",entry->id,entry->label,(&blank[labellen>33?33:labellen]));
    UiLcdHy28_PrintText(POS_MENU_IND_X, POS_MENU_IND_Y+(12*(pos)),out,m_clr,Black,0);
    switch(entry->kind) {
    case MENU_ITEM:
      UiDriverUpdateMenuLines(entry->number,mode,pos);
      break;
    case MENU_GROUP:
      strcpy(out,UiMenu_GroupIsUnfolded(entry)?"HIDE":"SHOW");
      UiLcdHy28_PrintTextRight(POS_MENU_CURSOR_X - 4, POS_MENU_IND_Y + (pos * 12), out, m_clr, Black, 0);       // yes, normal position
      break;
    }
  } else {
    UiLcdHy28_PrintText(POS_MENU_IND_X, POS_MENU_IND_Y+(12*(pos)),blank,m_clr,Black,0);
  }
}

void UiMenu_DisplayInitSlots(const MenuDescriptor* entry) {
  int idx;
  for (idx=0;idx < MENUSIZE;idx++) {
    UiMenu_FillSlotWithEntry(&menu[idx],entry);
    entry = UiMenu_NextMenuEntry(entry);
  }
}
void UiMenu_DisplayInitSlotsBackwards(const MenuDescriptor* entry) {
  int idx;
  for (idx=MENUSIZE;idx > 0;idx--) {
    UiMenu_FillSlotWithEntry(&menu[idx-1],entry);
    entry = UiMenu_PrevMenuEntry(entry);
  }
}


void UiMenu_DisplayMoveSlotsBackwards(int16_t change) {
  int idx;
  int dist = (change % MENUSIZE);
  int screens = change / MENUSIZE;
  for (idx = 0; idx < screens; idx++) {
    const MenuDescriptor *prev = UiMenu_PrevMenuEntry(menu[0].entryItem);
    if (prev != NULL) {
      UiMenu_DisplayInitSlotsBackwards(prev);
    } else {
      // we stop here, since no more previous elements.
      // TODO: Decide if roll over, in this case we would have to get very last element and
      // then continue from there.
      dist = 0;
      break;
    }
  }

  if (dist != 0) {
    for (idx = MENUSIZE-dist; idx > 0; idx--) {
      UiMenu_FillSlotWithEntry(&menu[MENUSIZE-idx],menu[MENUSIZE-(dist+idx)].entryItem);
    }

    for (idx = MENUSIZE-dist;idx >0; idx--) {
      UiMenu_FillSlotWithEntry(&menu[idx-1],UiMenu_PrevMenuEntry(menu[idx].entryItem));
    }
  }
}
void UiMenu_DisplayMoveSlotsForward(int16_t change) {
  int idx;
  int dist = (change % MENUSIZE);
  int screens = change / MENUSIZE;

  // first jump screens. we have to iterate through the menu structure one by one
  // in order to respect fold/unfold state etc.
  for (idx = 0; idx < screens; idx++) {
    const MenuDescriptor *next = UiMenu_NextMenuEntry(menu[MENUSIZE-1].entryItem);
    if (next != NULL) {
      UiMenu_DisplayInitSlots(next);
    } else {
      // stop here
      // TODO: Rollover?
      dist = 0;
      break;
    }
  }
  if (dist != 0) {
    for (idx = 0; idx < MENUSIZE-dist; idx++) {
      UiMenu_FillSlotWithEntry(&menu[idx],menu[dist+idx].entryItem);
    }
    for (idx = MENUSIZE-dist;idx < MENUSIZE; idx++) {
      UiMenu_FillSlotWithEntry(&menu[idx],UiMenu_NextMenuEntry(menu[idx-1].entryItem));
    }
  }
}

bool init_done = false;

void UiMenu_DisplayInitMenu(uint16_t mode) {
  if (init_done == false ) {
    UiMenu_DisplayInitSlots(&baseGroup[0]);
    init_done = true;
  }
  // UiMenu_DisplayMoveSlotsForward(6);
  // UiMenu_DisplayMoveSlotsForward(3);
  // UiMenu_DisplayMoveSlotsBackwards(10);
  int idx;
  for (idx = 0; idx < MENUSIZE; idx++) {
    UiMenu_UpdateMenuEntry(menu[idx].entryItem,mode, idx);
  }
}

void UiMenu_ShowSystemInfo() {
  uint32_t   m_clr;

  char out[32];
  char* outs;
  m_clr = White;
  float suf = ui_si570_get_startup_frequency();
  int vorkomma = (int)(suf);
  int nachkomma = (int) roundf((suf-vorkomma)*10000);
  static const char* display_types[] = {
      "                ",
      "HY28A SPI Mode  ",
      "HY28B SPI Mode  ",
      "HY28A/B parallel"
  };

  sprintf(out,"LCD Display  : %s",display_types[ts.display_type]);

  UiLcdHy28_PrintText(POS_MENU_IND_X, POS_MENU_IND_Y+0,out,m_clr,Black,0);
  sprintf(out,"SI570        : %xh / %u.%04u MHz",(os.si570_address >> 1),vorkomma,nachkomma);
  UiLcdHy28_PrintText(POS_MENU_IND_X, POS_MENU_IND_Y+12,out,m_clr,Black,0);
  switch (ts.ser_eeprom_type){
  case 0:
    outs = "n/a             ";
    break;
  case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15:
    outs = "incompatible    ";
    break;
  case 16:
    outs = "24xx512     64KB";
    break;
  case 17:
    outs = "24xx1025   128KB";
    break;
  case 18:
    outs = "24xx1026   128KB";
    break;
  case 19:
    outs = "24CM02     256KB";
    break;
  default:
    outs = "unknown         ";
    break;
  }
  sprintf(out,"%s%s","Serial EEPROM: ",outs);

  if(ts.ser_eeprom_in_use == 0)     // in use & data ok
    UiLcdHy28_PrintText(POS_MENU_IND_X, POS_MENU_IND_Y+24,out,Green,Black,0);
  if(ts.ser_eeprom_in_use == 0xFF)  // not in use
    UiLcdHy28_PrintText(POS_MENU_IND_X, POS_MENU_IND_Y+24,out,m_clr,Black,0);
  if(ts.ser_eeprom_in_use == 0x5)       // data not ok
    UiLcdHy28_PrintText(POS_MENU_IND_X, POS_MENU_IND_Y+24,out,Red,Black,0);
  if(ts.ser_eeprom_in_use == 0x10)  // EEPROM too small
    UiLcdHy28_PrintText(POS_MENU_IND_X, POS_MENU_IND_Y+24,out,Red,Black,0);

  if(ts.tp_present == 0)
    outs = "n/a             ";
  else
    outs = "XPT2046         ";
  sprintf(out,"%s%s","Touchscreen  : ",outs);
  UiLcdHy28_PrintText(POS_MENU_IND_X, POS_MENU_IND_Y+36,out,m_clr,Black,0);
  if(ts.rfmod_present == 0)
    outs = "n/a             ";
  else
    outs = "present         ";
  sprintf(out,"%s%s","RF Bands Mod : ",outs);
  UiLcdHy28_PrintText(POS_MENU_IND_X, POS_MENU_IND_Y+48,out,m_clr,Black,0);
  if(ts.vhfuhfmod_present == 0)
    outs = "n/a             ";
  else
    outs = "present         ";
  sprintf(out,"%s%s","VHF/UHF Mod  : ",outs);
  UiLcdHy28_PrintText(POS_MENU_IND_X, POS_MENU_IND_Y+60,out,m_clr,Black,0);
}


static char* base_screens[11][MENUSIZE] = {
{ //1
		"010-DSP NR Strength",
		"500-300Hz Center Freq.",
		"501-500HZ Center Freq.",
		"504-1.8k Center Freq.",
		"506-2.3k Center Freq.",
		"508-2.7k Filter"
},
{ // 2
		"512-3.6k Filter",
		"516-4.4k Filter",
		"521-6.0k Filter",
		"028-Wide Filt in CW Mode",
		"029-CW Filt in SSB Mode",
		"030-AM Mode"
},
{ // 3
		"031-LSB/USB Auto Select",
		"040-FM Mode",
		"041-FM Sub Tone Gen",
		"042-FM Sub Tone Det",
		"043-FM Tone Burst",
		"044-FM RX Bandwidth"
},
{ // 4
		"045-FM Deviation",
		"050-AGC Mode",
		"051-RF Gain",
		"052-Cust AGC (+=Slower)",
		"053-RX Codec Gain",
		"054-RX NB Setting"
},
{	// 5
		"055-RX/TX Freq Xlate",
		"060-Mic/Line Select",
		"061-Mic Input Gain",
		"062-Line Input Gain",
		"063-ALC Release Time",
		"064-TX PRE ALC Gain"
},
{	// 6
		"065-TX Audio Compress",
		"070-CW Keyer Mode",
		"071-CW Keyer Speed",
		"072-CW Sidetone Gain",
		"073-CW Side/Off Freq",
		"074-CW Paddle Reverse"
},
{	// 7
		"075-CW TX->RX Delay",
		"076-CW Freq. Offset",
		"090-TCXO Off/On/Stop",
		"091-TCXO Temp. (C/F)",
		"100-Spec Scope 1/Speed",
		"101-Spec/Wfall Filter"
},
{	// 8
		"102-Spec. Trace Colour",
		"103-Spec. Grid Colour",
		"104-Spec/Wfall ScaleClr",
		"105-Spec/Wfall 2x Magn",
		"106-Spec/Wfall AGC Adj.",
		"107-Spec Scope Ampl."
},
{	// 9
		"108-Spec/Wfall Line",
		"109-Scope/Waterfall",
		"110-Wfall Colours",
		"111-Wfall Step Size",
		"112-Wfall Brightness",
		"113-Wfall Contrast"
},
{	// 10
		"114-Wfall 1/Speed",
		"115-Scope NoSig Adj.",
		"116-Wfall NoSig Adj.",
		"117-Wfall Size",
		"197-Backup Config",
		"198-Restore Config"
},
{	// 11
		"199-Hardware Info               ",	//this screen must have blanks for cleaning hardware info page
		blankline,
		blankline,
		blankline,
		blankline,
		"000-Adjustment Menu             "
}
};


static char* conf_screens[20][MENUSIZE] = {
{ // 1
		"200-Step Size Marker",
		"201-Step Button Swap",
		"202-Band+/- Button Swap",
		"203-Transmit Disable",
		"204-Menu SW on TX disable",
		"205-Mute Line Out TX"
},
{	// 2
		"206-TX Mute Delay",
		"207-LCD Auto Blank",
		"208-Voltmeter Cal.",
		"209-Filter BW Display",
		"210-Max Volume",
		"211-Max RX Gain (0=Max)"
},
{ // 3
		"212-Key Beep",
		"213-Beep Frequency",
		"214-Beep Volume",
		"220-CAT Mode",
		"230-Freq. Calibrate",
		"231-Freq. Limit Disable"
},
{ // 4
		"232-MemFreq Lim Disable",
		"240-LSB RX IQ Bal.",
		"241-LSB RX IQ Phase",
		"242-USB RX IQ Bal.",
		"243-USB RX IQ Phase",
		"244-AM  RX IQ Bal."
},
{ // 5
		"245-FM  RX IQ Bal.",
		"250-LSB TX IQ Bal.",
		"251-LSB TX IQ Phase",
		"252-USB TX IQ Bal.",
		"253-USB TX IQ Phase",
		"254-AM  TX IQ Bal."
},
{ // 6
		"255-FM  TX IQ Bal.",
		"260-CW PA Bias (If >0 )",
		"261-PA Bias",
		"270-Disp. Pwr (mW)",
		"271-Pwr. Det. Null",
		"C01-2200m Coupling Adj."
},
{ // 7
		"C02-630m Coupling Adj.",
		"C03-160m Coupling Adj.",
		"C04-80m  Coupling Adj.",
		"C05-40m  Coupling Adj.",
		"C06-20m  Coupling Adj.",
		"C07-15m  Coupling Adj."
},
{	// 8
		"C08-6m   Coupling Adj.",
		"C09-2m   Coupling Adj.",
		"C10-70cm Coupling Adj.",
		"C11-23cm Coupling Adj.",
		"276-FWD/REV ADC Swap.",
		"280-XVTR Offs/Mult"
},
{ // 9
		"281-XVTR Offset",
		"P01-2200m 5W PWR Adjust",
		"P02-630m  5W PWR Adjust",
		"P03-160m  5W PWR Adjust",
		"P04-80m   5W PWR Adjust",
		"P05-60m   5W PWR Adjust"
},
{ // 10
		"P06-40m   5W PWR Adjust",
		"P07-30m   5W PWR Adjust",
		"P08-20m   5W PWR Adjust",
		"P09-17m   5W PWR Adjust",
		"P10-15m   5W PWR Adjust",
		"P11-12m   5W PWR Adjust"
},
{ // 11
		"P12-10m   5W PWR Adjust",
		"P13-6m    5W PWR Adjust",
		"P14-4m    5W PWR Adjust",
		"P15-2m    5W PWR Adjust",
		"P16-70cm  5W PWR Adjust",
		"P17-23cm  5W PWR Adjust"
},
{ // 12
		"O01-2200m Full PWR Adjust",
		"O02-630m  Full PWR Adjust",
		"O03-160m  Full PWR Adjust",
		"O04-80m   Full PWR Adjust",
		"O05-60m   Full PWR Adjust",
		"O06-40m   Full PWR Adjust"
},
{ // 13
		"O07-30m   Full PWR Adjust",
		"O08-20m   Full PWR Adjust",
		"O09-17m   Full PWR Adjust",
		"O10-15m   Full PWR Adjust",
		"O11-12m   Full PWR Adjust",
		"O12-10m   Full PWR Adjust"
},
{ // 14
		"O13-6m    Full PWR Adjust",
		"O14-4m    Full PWR Adjust",
		"O15-2m    Full PWR Adjust",
		"O16-70cm  Full PWR Adjust",
		"O17-23cm  Full PWR Adjust",
		"310-DSP NR BufLen"
},
{ // 15
		"311-DSP NR FFT NumTaps",
		"312-DSP NR Post-AGC",
		"313-DSP Notch ConvRate",
		"314-DSP Notch BufLen",
		"315-DSP Notch FFTNumTap",
		"320-NB  AGC T/C (<=Slow)"
},
{ // 16
		"330-AM  TX Audio Filter",
		"331-SSB TX Audio Filter",
		"335-Tune Power Level",
		"340-FFT Windowing",
		"341-Reset Ser EEPROM",
		"502-1.4k Filter",
},
{ // 17
		"503-1.6k Filter",
		"505-2.1k Filter",
		"507-2.5k Filter",
		"509-2.9k Filter",
		"510-3.2k Filter",
		"511-3.4k Filter"
},
{ // 18
		"513-3.8k Filter",
		"514-4.0k Filter",
		"515-4.2k Filter",
		"517-4.6k Filter",
		"518-4.8k Filter",
		"519-5.0k Filter"
},
{ // 19
		"520-5.5k Filter",
		"522-6.5k Filter",
		"523-7.0k Filter",
		"524-7.5k Filter",
		"525-8.0k Filter",
		"526-8.5k Filter"
},
{ // 20
		"527-9.0k Filter",
		"528-9.5k Filter",
		"529-10.0k Filter",
		"DSP NR (EXPERIMENTAL)",
		"400-CAT-IQ-FREQ-XLAT",
		" "
}
};



#define BandInfoGenerate(BAND,SUFFIX,NAME) { TX_POWER_FACTOR_##BAND##_DEFAULT, CONFIG_##BAND##SUFFIX##_5W_ADJUST, CONFIG_##BAND##SUFFIX##_FULL_POWER_ADJUST, BAND_FREQ_##BAND , BAND_SIZE_##BAND , NAME }

BandInfo bandInfo[] = {
				BandInfoGenerate(80,M,"80m") ,
				BandInfoGenerate(60,M,"60m"),
				BandInfoGenerate(40,M,"40m"),
				BandInfoGenerate(30,M,"30m"),
				BandInfoGenerate(20,M,"20m"),
				BandInfoGenerate(17,M,"17m"),
				BandInfoGenerate(15,M,"15m"),
				BandInfoGenerate(12,M,"12m"),
				BandInfoGenerate(10,M,"10m"),
				BandInfoGenerate(6,M,"6m"),
				BandInfoGenerate(4,M,"4m"),
				BandInfoGenerate(2,M,"2m"),
				BandInfoGenerate(70,CM,"70cm"),
				BandInfoGenerate(23,CM,"23cm"),
				BandInfoGenerate(2200,M,"2200m"),
				BandInfoGenerate(630,M,"630m"),
				BandInfoGenerate(160,M,"160m"),
				{ 0, 0, 0, 0, 0, "Gen" } // Generic Band
};

bool __attribute__ ((noinline)) UiDriverMenuBandPowerAdjust(int var, uint8_t mode, uint8_t band_mode, uint8_t pa_level, char* options, uint32_t* clr_ptr) {
	volatile uint8_t* adj_ptr;
	adj_ptr = &ts.pwr_adj[pa_level == PA_LEVEL_FULL?ADJ_FULL_POWER:ADJ_5W][band_mode];

	bool tchange = false;
	if((ts.band == band_mode) && (ts.power_level == pa_level))	{
		tchange = UiDriverMenuItemChangeUInt8(var, mode, adj_ptr,
				TX_POWER_FACTOR_MIN,
				TX_POWER_FACTOR_MAX,
				bandInfo[band_mode].default_pf,
				1
		);

		if(tchange)	{		// did something change?
			UiDriverSetBandPowerFactor(ts.band);	// yes, update the power factor
			if(!ts.iq_freq_mode)	// Is translate mode *NOT* active?
				Codec_SidetoneSetgain();				// adjust the sidetone gain
		}
	}
	else	// not enabled
		*clr_ptr = Orange;
	//
	sprintf(options, "  %u", *adj_ptr);
	return tchange;
}

bool __attribute__ ((noinline))  UiDriverMenuBandRevCouplingAdjust(int var, uint8_t mode, uint8_t filter_band, volatile uint8_t* adj_ptr, char* options, uint32_t* clr_ptr) {
	bool tchange = false;
	if(ts.filter_band == filter_band)	{	// is this band selected?
		tchange = UiDriverMenuItemChangeUInt8(var, mode, adj_ptr,
				SWR_COUPLING_MIN,
				SWR_COUPLING_MAX,
				SWR_COUPLING_DEFAULT,
				1
		);
	}
	if((ts.txrx_mode != TRX_MODE_TX) || (ts.filter_band != filter_band))	// Orange if not in TX mode or NOT on this band
		*clr_ptr = Orange;
	sprintf(options, "  %u", *adj_ptr);
	return tchange;
}


void  __attribute__ ((noinline))  UiDriverMenuChangeFilter(uint8_t filter_id, bool changed) {
	if((ts.txrx_mode == TRX_MODE_RX) && (changed))	{	// set filter if changed
		if(ts.filter_id == filter_id)	{
			//UiDriverProcessActiveFilterScan();	// find next active filter
			UiDriverChangeFilter(0);
			UiDriverDisplayFilterBW();	// update on-screen filter bandwidth indicator
		}
	}
}


bool UiMenuHandleFilterConfig(int var, uint8_t mode, char* options, uint32_t* clr_ptr, uint8_t bwId) {
  bool fchange = false;
  if (bwId < AUDIO_FILTER_NUM) {
    FilterDescriptor *filter = &FilterInfo[bwId];
    if(ts.dmod_mode != DEMOD_FM)    {
      fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.filter_select[bwId],
          0,
          filter->configs_num-1,
          filter->config_default,
          1
      );
      if(ts.filter_id != bwId) {
        *clr_ptr = Orange;
      }
    }
    else                // show disabled if in FM
      *clr_ptr = Red;

    if (ts.filter_select[bwId] == 0)    {
      *clr_ptr = Red;
    }
    strcpy(options,
        (ts.filter_select[bwId] < filter->configs_num)?
        filter->config[ts.filter_select[bwId]].label:
        "UNDEFINED"
        );
    UiDriverMenuChangeFilter(bwId,fchange);
  }
  return fchange;
}

//
//
//
//*----------------------------------------------------------------------------
//* Function Name       : UiDriverUpdateMenu
//* Object              : Display and change menu items
//* Input Parameters    : mode:  0=update all, 1=update current item, 2=go to next screen, 3=restore default setting for selected item
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
//
void UiDriverUpdateMenu(uchar mode)
{
#ifdef NEWMENU
  UiMenu_DisplayInitMenu(mode);
  return;
#endif
  uchar var;
  bool  update_vars;
  static uchar screen_disp = 1;	// used to detect screen display switching and prevent unnecessary blanking
  uint32_t	m_clr, c_clr;
  static	int	menu_var_changed = 99;
  uchar warn = 0;
  int offset = 50;

  m_clr = Yellow;
  c_clr = Cyan;

  update_vars = 0;

  if(mode > 3) {
    if(mode == 9) {
      UiMenu_ShowSystemInfo();
    }
  } else  {
    uint8_t old_screen_disp = screen_disp;
    if (ts.menu_item < MAX_MENU_ITEM) {
      screen_disp = 1+ (ts.menu_item / MENUSIZE);
    } else {
      screen_disp = offset+1+ ((ts.menu_item - MAX_MENU_ITEM) / MENUSIZE);
    }

    if(old_screen_disp != screen_disp) {	// redraw if this screen wasn't already displayed
      UiSpectrumClearDisplay();
    }
    update_vars = 1;
    //
    // ****************   Main Menu  ***************
    //
    if(ts.menu_item < MAX_MENU_ITEM)
    {		// Is this part of the main menu?
      int i;
      for (i=MENUSIZE*(screen_disp-1); i < MENUSIZE*(screen_disp); i++ )
      {
        bool show = !(ts.ser_eeprom_in_use != 0 && (screen_disp == 10) && (i % MENUSIZE == 2 || i % MENUSIZE == 3));
        // this takes care of the removing 2 items if serial eeprom is not fitted
        if (show)
          UiLcdHy28_PrintText(POS_MENU_IND_X, POS_MENU_IND_Y+(12*(i%MENUSIZE)),base_screens[screen_disp-1][(i%MENUSIZE)],m_clr,Black,0);
      }
    }
    //
    // ****************   Radio Calibration Menu  ***************
    //
    else
    {		// Is this part of the radio configuration menu?
      int i;
      for (i=MENUSIZE*(screen_disp-offset-1); i < MENUSIZE*(screen_disp-offset); i++ )
        UiLcdHy28_PrintText(POS_MENU_IND_X, POS_MENU_IND_Y+(12*(i%MENUSIZE)),conf_screens[screen_disp-offset-1][(i%MENUSIZE)],c_clr,Black,0);
    }


    if(ts.menu_var_changed)	{		// show warning if variable has changed
      if(warn != 1)
        UiLcdHy28_PrintText(POS_SPECTRUM_IND_X - 2, POS_SPECTRUM_IND_Y + 60, " Save settings using POWER OFF!  ", Orange, Black, 0);
      warn = 1;
    }
    else	{					// erase warning by using the same color as the background
      if(warn != 2)
        UiLcdHy28_PrintText(POS_SPECTRUM_IND_X - 2, POS_SPECTRUM_IND_Y + 60, " CW impaired when in MENU mode!  ", Grey, Black, 0);
      warn = 2;
    }
    //

    //
    //
    if(ts.menu_var != menu_var_changed)	{	// do additional validation to avoid additional updates on display
      update_vars = 1;
    }
    menu_var_changed = ts.menu_var;
    //
    //
    // These functions are used to scan the individual menu items and display the items.
    // In each of the FOR loops below, make CERTAIN that the precise number of items are included for each menu!
    //

    //	if(((mode == 0) && (sd.use_spi) && (screen_disp != screen_disp_old)) || (((mode == 0) && (!sd.use_spi))) || update_vars)	{		// display all items and their current settings
    // but minimize updates if the LCD is using an SPI interface
    if((mode == 0) || update_vars)	{
      update_vars = 0;
      uint32_t menu_num = ts.menu_item / MENUSIZE;
      // calculate screen number simply by dividing by MENUSIZE
      // then loop over the correct function to display the items
      if (ts.menu_item < MAX_MENU_ITEM) {
        for(var = menu_num * MENUSIZE; (var < ((menu_num+1) * MENUSIZE)) && var < MAX_MENU_ITEM; var++) {
          UiDriverUpdateMenuLines(var, 0, -1);
        }
      }
      //
      // *** ADJUSTMENT MENU ***
      //
      else {	// Is this one of the radio configuration items?
        for(var = menu_num * MENUSIZE; (var < (menu_num+1) * MENUSIZE) && var < (MAX_MENU_ITEM + MAX_RADIO_CONFIG_ITEM); var++) {
          UiDriverUpdateConfigMenuLines(var-MAX_MENU_ITEM, 0);
        }
      }
    }

    //
    //	screen_disp_old = screen_disp;
    //
    if(mode == 1)	{	// individual item selected/changed
      if(ts.menu_item < MAX_MENU_ITEM)					// main menu item
        UiDriverUpdateMenuLines(ts.menu_item, 1, -1);
      else												// "adjustment" menu item
        UiDriverUpdateConfigMenuLines(ts.menu_item-MAX_MENU_ITEM, 1);
    }
    else if(mode == 3)	{	// restore default setting for individual item
      if(ts.menu_item < MAX_MENU_ITEM)					// main menu item
        UiDriverUpdateMenuLines(ts.menu_item, 3, -1);
      else												// "adjustment" menu item
        UiDriverUpdateConfigMenuLines(ts.menu_item-MAX_MENU_ITEM,3);
    }
  }
}
//
//
//*----------------------------------------------------------------------------
//* Function Name       : UiDriverUpdateMenuLines
//* Object              : Display and and change line items
//* Input Parameters    : index:  Line to display  mode:  0=display/update 1=change item 3=set default, pos > -1 use this line as position
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
//
static void UiDriverUpdateMenuLines(uchar index, uchar mode, int pos)
{
	char options[32];
	ulong opt_pos;			// y position of option cursor
	static ulong opt_oldpos = 999;	// y position of option cursor, previous
	uchar select;
	ulong	clr;
	uchar temp_var;
	int	a, b;
	int var;
	float tcalc;
	bool	fchange = 0;
	uchar	temp_sel;		// used as temporary holder during selection
	// bool	disp_shift = 0;	// TRUE if option display is to be shifted to the left to allow more options

	clr = White;		// color used it display of adjusted options

	if(mode == 0)	{	// are we in update/display mode?
		select = index;	// use index passed from calling function
		var = 0;		// prevent any change of variable
	}
	else	{			// this is "change" mode
		select = ts.menu_item;	// item selected from encoder
		var = ts.menu_var;		// change from encoder
		ts.menu_var = 0;		// clear encoder change detect
	}
	strcpy(options, "ERROR");	// pre-load to catch error condition
	//
	opt_pos = select % MENUSIZE; // calculate position from menu item number

	if (pos > -1) {
	  opt_pos = pos;
	}

	switch(select)	{		//  DSP_NR_STRENGTH_MAX
	case MENU_DSP_NR_STRENGTH:	// DSP Noise reduction strength

		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.dsp_nr_strength,
				0,
				DSP_NR_STRENGTH_MAX,
				DSP_NR_STRENGTH_DEFAULT,
				1
				);
		if(fchange)
		{		// did it change?
			if(ts.dsp_active & 1)	// only change if DSP active
				audio_driver_set_rx_audio_filter();
		}
		//
		if(!(ts.dsp_active & 1))	// make red if DSP not active
			clr = Orange;
		else	{
			if(ts.dsp_nr_strength >= DSP_STRENGTH_RED)
				clr = Red;
			else if(ts.dsp_nr_strength >= DSP_STRENGTH_ORANGE)
				clr = Orange;
			else if(ts.dsp_nr_strength >= DSP_STRENGTH_YELLOW)
				clr = Yellow;
		}
		//
		sprintf(options, "  %u", ts.dsp_nr_strength);
		break;
	//
	case MENU_300HZ_SEL:	// 300 Hz filter select
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_300HZ);
		break;
	case MENU_500HZ_SEL:	// 500 Hz filter select
	  UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_500HZ);
	  break;
	case MENU_1K8_SEL:	// 1.8 kHz filter select
	  UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_1P8KHZ);
	  break;

	case MENU_2K3_SEL: // 2.3 kHz filter select
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_2P3KHZ);
		break;
	case MENU_2K7_SEL: // 2.7 kHz filter select
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_2P7KHZ);
		break;
	case MENU_3K6_SEL: // 3.6 kHz filter select
	  UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_3P6KHZ);
		break;
	case MENU_4K4_SEL:	//
	  UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_4P4KHZ);
	  break;
	case MENU_6K0_SEL:	//
      UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_6P0KHZ);
	  break;


/*	case MENU_WIDE_SEL: // Wide filter select
		if(ts.dmod_mode != DEMOD_FM)	{
			fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.filter_wide_select,
					0,
					WIDE_FILTER_MAX,
					WIDE_FILTER_10K,
					1
			);
			if((ts.filter_id != AUDIO_WIDE))	// make orange if NOT in "Wide" mode
				clr = Orange;
		}
		else				// show disabled if in FM
			clr = Red;
		//
		switch(ts.filter_wide_select)	{
			case WIDE_FILTER_5K_AM:
				strcpy(options, "  5kHz AM");
				break;
			case WIDE_FILTER_6K_AM:
				strcpy(options, "  6kHz AM");
				break;
			case WIDE_FILTER_7K5_AM:
				strcpy(options, "7.5kHz AM");
				break;
			case WIDE_FILTER_10K_AM:
				strcpy(options, "10 kHz AM");
				break;
			case WIDE_FILTER_5K:
				strcpy(options, "     5kHz");
				break;
			case WIDE_FILTER_6K:
				strcpy(options, "     6kHz");
				break;
			case WIDE_FILTER_7K5:
				strcpy(options, "    7.5kHz");
				break;
			case WIDE_FILTER_10K:
			default:
				strcpy(options, "     10kHz");
				break;
		}
		//
		if((ts.filter_id == AUDIO_WIDE) && fchange)	{
			//UiDriverProcessActiveFilterScan();	// find next active filter
			UiDriverChangeFilter(0);
			UiCalcRxPhaseAdj();						// update Hilbert/LowPass filter setting
			UiDriverDisplayFilterBW();	// update on-screen filter bandwidth indicator
		}
		//
		// disp_shift = 1;		// move the options to the left slightly
		break;
	*/
	case MENU_CW_WIDE_FILT: // CW mode wide filter enable/disable
		UiDriverMenuItemChangeDisableOnOff(var, mode, &ts.filter_cw_wide_disable,0,options,&clr);

		if((ts.filter_id != DEMOD_CW))	// make orange if NOT in "Wide" mode
			clr = Orange;
		break;
	//
	case MENU_SSB_NARROW_FILT: // SSW mode narrow filter enable/disable
		UiDriverMenuItemChangeDisableOnOff(var, mode, &ts.filter_ssb_narrow_disable,0,options,&clr);

		if((ts.filter_id != DEMOD_CW))	// make orange if NOT in "Wide" mode
					clr = Orange;

		break;
		//
	case MENU_AM_DISABLE: // AM mode enable/disable
		UiDriverMenuItemChangeDisableOnOff(var, mode, &ts.am_mode_disable,0,options,&clr);
		break;
		//
	case MENU_SSB_AUTO_MODE_SELECT:		// Enable/Disable auto LSB/USB select
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.lsb_usb_auto_select,
				0,
				AUTO_LSB_USB_MAX,
				AUTO_LSB_USB_OFF,
				1
				);
		if(ts.lsb_usb_auto_select == AUTO_LSB_USB_ON)		// LSB on bands < 10 MHz
			strcpy(options, "     ON");		// yes
		else if(ts.lsb_usb_auto_select == AUTO_LSB_USB_60M)	// USB on 60 meters?
			strcpy(options, "USB 60M");		// yes
		else
			strcpy(options, "    OFF");		// no (obviously!)
		break;
	//
	case MENU_FM_MODE_ENABLE:	// Enable/Disable FM
		if(ts.iq_freq_mode)	{
			temp_var = ts.misc_flags2 & 1;
			fchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
			if(fchange)	{
				if(temp_var)	// band up/down swap is to be enabled
					ts.misc_flags2 |= 1;		// FM is enabled
				else			// band up/down swap is to be disabled
					ts.misc_flags2 &= 0xfe;		// FM is disabled
			}

		}
		else	{	// translate mode is off - NO FM!!!
			strcpy(options, "OFF");		// Say that it is OFF!
			clr = Red;
		}
		break;
	case MENU_FM_GEN_SUBAUDIBLE_TONE:	// Selection of subaudible tone for FM transmission
		UiDriverMenuItemChangeUInt32(var, mode, &ts.fm_subaudible_tone_gen_select,
				0,
				NUM_SUBAUDIBLE_TONES,
				FM_SUBAUDIBLE_TONE_OFF,
				1
				);

		if(ts.fm_subaudible_tone_gen_select)	{	// tone select not zero (tone activated
			AudioManagement_CalcSubaudibleGenFreq();		// calculate frequency word
			a = (int)(ads.fm_subaudible_tone_gen_freq * 10);		// convert to integer, Hz*10
			b = a;
			a /= 10;		// remove 10ths of Hz
			a *= 10;		// "a" now has Hz*100 with 10ths removed
			b -= a;			// "b" now has 10ths of Hz
			a /= 10;		// "a" is back to units of Hz
			sprintf(options, "  %d.%dHz", a, b);
		}
		else	{							// tone is off
			sprintf(options, "     OFF");		// make it dislay "off"
			ads.fm_subaudible_tone_word = 0;	// set word to 0 to turn it off
		}
		//
		if(ts.dmod_mode != DEMOD_FM)	// make orange if we are NOT in FM mode
			clr = Orange;
		else if(ads.fm_subaudible_tone_det_freq > 200)		// yellow for tones above 200 Hz as they are more audible
			clr = Yellow;
		break;
	//
	case MENU_FM_DET_SUBAUDIBLE_TONE:	// Selection of subaudible tone for FM reception
		UiDriverMenuItemChangeUInt32(var, mode, &ts.fm_subaudible_tone_det_select,
					0,
					NUM_SUBAUDIBLE_TONES,
					FM_SUBAUDIBLE_TONE_OFF,
					1
					);
		//
		if(ts.fm_subaudible_tone_det_select)	{	// tone select not zero (tone activated
			AudioManagement_CalcSubaudibleDetFreq();		// calculate frequency word
			a = (int)(ads.fm_subaudible_tone_det_freq * 10);		// convert to integer, Hz*10
			b = a;
			a /= 10;		// remove 10ths of Hz
			a *= 10;		// "a" now has Hz*100 with 10ths removed
			b -= a;			// "b" now has 10ths of Hz
			a /= 10;		// "a" is back to units of Hz
			sprintf(options, "  %d.%dHz", a, b);
		}
		else	{							// tone is off
			sprintf(options, "     OFF");		// make it dislay "off"
			ads.fm_subaudible_tone_word = 0;	// set word to 0 to turn it off
		}
		//
		if(ts.dmod_mode != DEMOD_FM)	// make orange if we are NOT in FM
			clr = Orange;
		else if(ads.fm_subaudible_tone_det_freq > 200)		// yellow for tones above 200 Hz as they are more audible
			clr = Yellow;
		break;
	//
	case MENU_FM_TONE_BURST_MODE:
		UiDriverMenuItemChangeUInt8(var, mode, &ts.fm_tone_burst_mode,
					0,
					FM_TONE_BURST_MAX,
					FM_TONE_BURST_OFF,
					1
					);
		//
		if(ts.fm_tone_burst_mode == FM_TONE_BURST_1750_MODE)	{		// if it was 1750 Hz mode, load parameters
			ads.fm_tone_burst_active = 0;								// make sure it is turned off
			strcpy(options, "1750 Hz");
			ads.fm_tone_burst_word = FM_TONE_BURST_1750;
		}
		else if(ts.fm_tone_burst_mode == FM_TONE_BURST_2135_MODE)	{	// if it was 2135 Hz mode, load information
			ads.fm_tone_burst_active = 0;								// make sure it is turned off
			strcpy(options, "2135 Hz");
			ads.fm_tone_burst_word = FM_TONE_BURST_2135;
		}
		else	{												// anything else, turn it off
			strcpy(options, "    OFF");
			ads.fm_tone_burst_word = FM_TONE_BURST_OFF;
			ads.fm_tone_burst_active = 0;
		}
		//
		if(ts.dmod_mode != DEMOD_FM)	// make orange if we are NOT in FM
			clr = Orange;
		break;
	//
	case MENU_FM_RX_BANDWIDTH:
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.fm_rx_bandwidth,
						0,
						FM_RX_BANDWIDTH_MAX,
						FM_BANDWIDTH_DEFAULT,
						1
						);
				//
		if(ts.fm_rx_bandwidth == FM_RX_BANDWIDTH_7K2)	{		// if it is 7.2 kHz FM RX bandwidth
			strcpy(options, "7.5kHz");
		}
		else if(ts.fm_rx_bandwidth == FM_RX_BANDWIDTH_12K)	{	// if it was 12 kHz bandwidth
			strcpy(options, "12 kHz");
		}
//		else if(ts.fm_rx_bandwidth == FM_RX_BANDWIDTH_15K)	{	// if it was 15 kHz bandwidth
//			strcpy(options, "15 kHz");
//		}
		else	{						// it was anything else (10 kHz - hope!)
			strcpy(options, "10 kHz");
		}
		//
		if(fchange)	{			// was the bandwidth changed?
			AudioFilter_CalcRxPhaseAdj();			// yes - update the filters!
			UiDriverChangeFilter(1);	// update display of filter bandwidth (numerical) on screen only
		}
		//
		if(ts.dmod_mode != DEMOD_FM)	// make orange if we are NOT in FM
			clr = Orange;
		break;
	//
	case MENU_FM_DEV_MODE:	// Select +/- 2.5 or 5 kHz deviation on RX and TX
		if(ts.iq_freq_mode)	{
			temp_var = ts.misc_flags2 & 2;
			fchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
			if(fchange)	{
				if(temp_var)	// band up/down swap is to be enabled
					ts.misc_flags2 |= 2;		// set 5 kHz mode
				else			// band up/down swap is to be disabled
					ts.misc_flags2 &= 0xfd;		// set 2.5 kHz mode
			}
			//
			if(ts.misc_flags2 & 2)				// Check state of bit indication 2.5/5 kHz
				strcpy(options, "+-5k (Wide)");		// Bit is set - 5 kHz
			else
				strcpy(options, "+-2k5 (Nar)");		// Not set - 2.5 kHz
		}
		else	{	// translate mode is off - NO FM!!!
			strcpy(options, "  OFF");		// Say that it is OFF!
			clr = Red;
		}
		//
		// disp_shift = 1;
		break;
	//
	case MENU_AGC_MODE:	// AGC mode
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.agc_mode,
						0,
						AGC_MAX_MODE,
						AGC_DEFAULT,
						1
						);
		if(ts.agc_mode == AGC_SLOW)
			strcpy(options, " SLOW");
		else if(ts.agc_mode == AGC_MED)
			strcpy(options, "  MED");
		else if(ts.agc_mode == AGC_FAST)
			strcpy(options, "  FAST");
		else if(ts.agc_mode == AGC_OFF)	{
			strcpy(options, "MANUAL");
			clr = Red;
		}
		else if(ts.agc_mode == AGC_CUSTOM)
			strcpy(options, "CUSTOM");
		//
		if(fchange)	{
			// now set the AGC
			AudioManagement_CalcAGCDecay();	// initialize AGC decay ("hang time") values
		}
		//
		if(ts.txrx_mode == TRX_MODE_TX)	// Orange if in TX mode
			clr = Orange;
		break;
	//
	case MENU_RF_GAIN_ADJ:		// RF gain control adjust
		fchange = UiDriverMenuItemChangeInt(var, mode, &ts.rf_gain,
						0,
						MAX_RF_GAIN,
						DEFAULT_RF_GAIN,
						1
						);
		if(fchange)	{
			AudioManagement_CalcRFGain();
		}
		//
		if(ts.rf_gain < 20)
			clr = Red;
		else if(ts.rf_gain < 30)
			clr = Orange;
		else if(ts.rf_gain < 40)
			clr = Yellow;
		else
			clr = White;
		//
		if(fchange)		// did RFGain get changed?
			UiDriverChangeRfGain(0);	// yes, change on-screen RF gain setting
		//
		sprintf(options, "  %d", ts.rf_gain);
		break;
		// RX Codec gain adjust
	case MENU_CUSTOM_AGC:		// Custom AGC adjust
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.agc_custom_decay,
						0,
						AGC_CUSTOM_MAX,
						AGC_CUSTOM_DEFAULT,
						1
						);
		//
		if(fchange)	{
			if(ts.agc_custom_decay > AGC_CUSTOM_MAX)
				ts.agc_custom_decay = AGC_CUSTOM_MAX;
			// now set the custom AGC - if in custom mode
			if(ts.agc_mode == AGC_CUSTOM)	{
				tcalc = (float)ts.agc_custom_decay;	// use temp var "tcalc" as audio function
				tcalc += 30;			// can be called mid-calculation!
				tcalc /= 10;
				tcalc *= -1;
				tcalc = powf(10, tcalc);
				ads.agc_decay = tcalc;
			}
		}
		//
		if((ts.txrx_mode == TRX_MODE_TX) || (ts.agc_mode != AGC_CUSTOM))	// Orange if in TX mode
			clr = Orange;
		else if(ts.agc_custom_decay <= AGC_CUSTOM_FAST_WARNING)				// Display in red if setting may be too fast
			clr = Red;
		//
		sprintf(options, "  %d", ts.agc_custom_decay);
		break;
	// A/D Codec Gain/Mode setting/adjust
	case MENU_CODEC_GAIN_MODE:
		UiDriverMenuItemChangeUInt8(var, mode, &ts.rf_codec_gain,
						0,
						MAX_RF_CODEC_GAIN_VAL,
						DEFAULT_RF_CODEC_GAIN_VAL,
						1
						);

		if(ts.rf_codec_gain == 9)
			strcpy(options, " AUTO");
		else	{	// if anything other than "Auto" give a warning in RED
			sprintf(options,"> %u <", ts.rf_codec_gain);
			clr = Red;
		}
		break;
		//
	case MENU_NOISE_BLANKER_SETTING:
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.nb_setting,
						0,
						MAX_NB_SETTING,
						0,
						1
						);
		//
		if(ts.nb_setting >= NB_WARNING3_SETTING)
			clr = Red;		// above this value, make it red
		else if(ts.nb_setting >= NB_WARNING2_SETTING)
			clr = Orange;		// above this value, make it orange
		else if(ts.nb_setting >= NB_WARNING1_SETTING)
			clr = Yellow;		// above this value, make it yellow
		//
		sprintf(options,"   %u", ts.nb_setting);
		//
		break;
	//
	case MENU_RX_FREQ_CONV:		// Enable/Disable receive frequency conversion
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.iq_freq_mode,
						0,
						FREQ_IQ_CONV_MODE_MAX,
						FREQ_IQ_CONV_MODE_DEFAULT,
						1
						);
		// disp_shift = 1;
		if(!ts.iq_freq_mode)	{
			sprintf(options,">> OFF! <<");
			clr = Red3;
		}
		else if(ts.iq_freq_mode == FREQ_IQ_CONV_P6KHZ)	{
			sprintf(options,"RX  +6kHz");
		}
		else if(ts.iq_freq_mode == FREQ_IQ_CONV_M6KHZ)	{
			sprintf(options,"RX  -6kHz");
		}
		else if(ts.iq_freq_mode == FREQ_IQ_CONV_P12KHZ)	{
			sprintf(options,"RX +12kHz");
		}
		else if(ts.iq_freq_mode == FREQ_IQ_CONV_M12KHZ)	{
			sprintf(options,"RX -12kHz");
		}
		//
		//
		if(fchange)	{	// update parameters if changed
			UiDriverUpdateFrequency(2,0);	// update frequency display without checking encoder, unconditionally updating synthesizer
		}
		//
		break;
	//
	case MENU_MIC_LINE_MODE:	// Mic/Line mode
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.tx_audio_source,
						0,
						TX_AUDIO_MAX_ITEMS,
						TX_AUDIO_MIC,
						1
						);
		if(ts.tx_audio_source == TX_AUDIO_MIC) {
			strcpy(options, "    MIC");
		} else if(ts.tx_audio_source == TX_AUDIO_LINEIN_L) {
			strcpy(options, " LINE-L");
		} else if(ts.tx_audio_source == TX_AUDIO_LINEIN_R) {
			strcpy(options, " LINE-R");
		} else if(ts.tx_audio_source == TX_AUDIO_DIG) {
			strcpy(options, " DIGITAL");
		} else if(ts.tx_audio_source == TX_AUDIO_DIGIQ) {
			strcpy(options, " DIG I/Q");
		}

		if(fchange)	{		// if there was a change, do update of on-screen information
			if(ts.dmod_mode == DEMOD_CW)
				UiDriverChangeKeyerSpeed(0);
			else
				{
				UiDriverChangeAudioGain(0);
				UiDriverUpdateMenu(0);
				}
		}

		if((!ts.cat_mode_active) && ts.tx_audio_source == TX_AUDIO_DIG) {
			// RED if CAT is not enabled
			clr = Red;
		}

		break;
	//
	case MENU_MIC_GAIN:	// Mic Gain setting

		if(ts.tx_audio_source == TX_AUDIO_MIC)	{	// Allow adjustment only if in MIC mode
			fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.tx_gain[TX_AUDIO_MIC],
							MIC_GAIN_MIN,
							MIC_GAIN_MAX,
							MIC_GAIN_DEFAULT,
							1
							);
		}
		if((ts.txrx_mode == TRX_MODE_TX) && (fchange))	{		// only adjust the hardware if in TX mode (it will kill RX otherwise!)
			Codec_MicBoostCheck();
		}
		//
		if(fchange)	{		// update on-screen info if there was a change
			if(ts.dmod_mode == DEMOD_CW)
				UiDriverChangeKeyerSpeed(0);
			else
				UiDriverChangeAudioGain(0);
		}
		//
		if(ts.tx_audio_source != TX_AUDIO_MIC) {	// Orange if not in MIC-IN mode
			clr = Orange;
		}
		//
		sprintf(options, "   %u", ts.tx_gain[TX_AUDIO_MIC]);
		break;
	//
	case MENU_LINE_GAIN:	// Line Gain setting

		// TODO: Revise, since it now changes the currently selected tx source setting
		if(ts.tx_audio_source != TX_AUDIO_MIC)	{	// Allow adjustment only if in line-in mode
			fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.tx_gain[ts.tx_audio_source],
							LINE_GAIN_MIN,
							LINE_GAIN_MAX,
							LINE_GAIN_DEFAULT,
							1
							);

		}
		//
		if(fchange)	{		// update on-screen info and codec if there was a change
			if(ts.dmod_mode == DEMOD_CW)
				UiDriverChangeKeyerSpeed(0);
			else	{		// in voice mode
				UiDriverChangeAudioGain(0);
				if(ts.txrx_mode == TRX_MODE_TX)		// in transmit mode?
					// TODO: Think about this, this is a hack
					Codec_Line_Gain_Adj(ts.tx_gain[TX_AUDIO_LINEIN_L]);		// change codec gain
			}
		}
		//
		if(ts.tx_audio_source == TX_AUDIO_MIC)	// Orange if in MIC mode
		    {
		    clr = Orange;
		    sprintf(options, "  %u", ts.tx_gain[TX_AUDIO_LINEIN_L]);
		    }
		else
		    sprintf(options, "  %u", ts.tx_gain[ts.tx_audio_source]);
		break;
	//
	case MENU_ALC_RELEASE:		// ALC Release adjust

		if(ts.tx_comp_level == TX_AUDIO_COMPRESSION_MAX)	{
			fchange = UiDriverMenuItemChangeUInt32(var, mode, &ts.alc_decay_var,
							0,
							ALC_DECAY_MAX,
							ALC_DECAY_DEFAULT,
							1
							);
			if(fchange)	{		// value changed?  Recalculate
				AudioManagement_CalcALCDecay();
			}
		}
		else			// indicate RED if "Compression Level" below was nonzero
			clr = Red;
		//
		if(ts.tx_comp_level == TX_AUDIO_COMPRESSION_SV)	// in "selectable value" mode?
			ts.alc_decay = ts.alc_decay_var;	// yes, save new value
		//
		sprintf(options, "  %d", (int)ts.alc_decay_var);
		break;
	//
	case MENU_ALC_POSTFILT_GAIN:		// ALC TX Post-filter gain (Compressor level)
		if(ts.tx_comp_level == TX_AUDIO_COMPRESSION_MAX)	{
			fchange = UiDriverMenuItemChangeUInt32(var, mode, &ts.alc_tx_postfilt_gain_var,
							ALC_POSTFILT_GAIN_MIN,
							ALC_POSTFILT_GAIN_MAX,
							ALC_POSTFILT_GAIN_DEFAULT,
							1
							);
			//
			if(fchange)	{
				if(ts.dmod_mode != DEMOD_CW)	// In voice mode?
					UiDriverChangeCmpLevel(0);	// update on-screen display of compression level
			}
		}
		else			// indicate RED if "Compression Level" below was nonzero
			clr = Red;
		//
		if(ts.tx_comp_level == TX_AUDIO_COMPRESSION_SV)	// in "selectable value" mode?
			ts.alc_tx_postfilt_gain = ts.alc_tx_postfilt_gain_var;	// yes, save new value
		//
		sprintf(options, "  %d", (int)ts.alc_tx_postfilt_gain_var);
		break;
	case MENU_TX_COMPRESSION_LEVEL:		// ALC TX Post-filter gain (Compressor level)
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.tx_comp_level,
						0,
						TX_AUDIO_COMPRESSION_MAX,
						TX_AUDIO_COMPRESSION_DEFAULT,
						1
						);
		//
		if(fchange)	{
			AudioManagement_CalcTxCompLevel();			// calculate parameters for selected amount of compression
			//
			if(ts.dmod_mode != DEMOD_CW)	// In voice mode?
				UiDriverChangeCmpLevel(0);	// update on-screen display of compression level
		}
		//
		if(ts.tx_comp_level < TX_AUDIO_COMPRESSION_SV)	// 	display numbers for all but the highest value
			sprintf(options,"  %d",ts.tx_comp_level);
		else					// show "SV" (Stored Value) for highest value
			strcpy(options, " SV");
		break;
		//
	case MENU_KEYER_MODE:	// Keyer mode
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.keyer_mode,
						0,
						CW_MODE_STRAIGHT,
						CW_MODE_IAM_B,
						1
						);
		//
		if(ts.keyer_mode == CW_MODE_IAM_B)
			strcpy(options, "IAM_B");
		else if(ts.keyer_mode == CW_MODE_IAM_A)
			strcpy(options, "IAM_A");
		else if(ts.keyer_mode == CW_MODE_STRAIGHT)
			strcpy(options, "STR_K");
		break;
	//
	case MENU_KEYER_SPEED:	// keyer speed
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.keyer_speed,
						MIN_KEYER_SPEED,
						MAX_KEYER_SPEED,
						DEFAULT_KEYER_SPEED,
						1
						);
		//
		if(fchange)	{		// did it get changed?
			if(ts.dmod_mode == DEMOD_CW)		// yes, update on-screen info
				UiDriverChangeKeyerSpeed(0);
			else
				UiDriverChangeAudioGain(0);
		}
		//
		sprintf(options, "  %u", ts.keyer_speed);
		break;
	//
	case MENU_SIDETONE_GAIN:	// sidetone gain
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.st_gain,
						0,
						SIDETONE_MAX_GAIN,
						DEFAULT_SIDETONE_GAIN,
						1
						);
		if(fchange)	{		// did it change?
			if(ts.dmod_mode == DEMOD_CW)
				UiDriverChangeStGain(0);		// update on-screen display of sidetone gain
			else
				UiDriverChangeCmpLevel(0);
		}
		sprintf(options, "  %u", ts.st_gain);
		break;
	//
	case MENU_SIDETONE_FREQUENCY:	// sidetone frequency
		fchange = UiDriverMenuItemChangeUInt32(var, mode, &ts.sidetone_freq,
						CW_SIDETONE_FREQ_MIN,
						CW_SIDETONE_FREQ_MAX,
						CW_SIDETONE_FREQ_DEFAULT,
						10
						);

		if((ts.dmod_mode == DEMOD_CW) && (fchange))	{
			softdds_setfreq((float)ts.sidetone_freq,ts.samp_rate,0);
			UiDriverUpdateFrequency(2,0);	// update frequency display without checking encoder, unconditionally updating synthesizer
		}
		//
		sprintf(options, "  %uHz", (uint)ts.sidetone_freq);
		break;
	//
	case MENU_PADDLE_REVERSE:	// CW Paddle reverse
		UiDriverMenuItemChangeEnableOnOff(var, mode, &ts.paddle_reverse,0,options,&clr);
		break;
	case MENU_CW_TX_RX_DELAY:	// CW TX->RX delay
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.cw_rx_delay,
						0,
						CW_RX_DELAY_MAX,
						CW_RX_DELAY_DEFAULT,
						1
						);

		sprintf(options, "  %u", ts.cw_rx_delay);
		break;
	//
	case MENU_CW_OFFSET_MODE:	// CW offset mode (e.g. USB, LSB, etc.)
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.cw_offset_mode,
						0,
						CW_OFFSET_MAX,
						CW_OFFSET_MODE_DEFAULT,
						1
						);
		//
		switch(ts.cw_offset_mode)	{
			case CW_OFFSET_USB_TX:
				sprintf(options, "        USB");
				break;
			case CW_OFFSET_LSB_TX:
				sprintf(options, "        LSB");
				break;
			case CW_OFFSET_USB_RX:
				sprintf(options, "   USB DISP");
				break;
			case CW_OFFSET_LSB_RX:
				sprintf(options, "   LSB DISP");
				break;
			case CW_OFFSET_USB_SHIFT:
				sprintf(options, "  USB SHIFT");
				break;
			case CW_OFFSET_LSB_SHIFT:
				sprintf(options, "  LSB SHIFT");
				break;
			case CW_OFFSET_AUTO_TX:
				sprintf(options, "AUT USB/LSB");
				break;
			case CW_OFFSET_AUTO_RX:
				sprintf(options, "  AUTO DISP");
				break;
			case CW_OFFSET_AUTO_SHIFT:
				sprintf(options, " AUTO SHIFT");
				break;
			default:
				sprintf(options, "     ERROR!");
				break;
		}
		//
		if(fchange)	{	// update parameters if changed
			UiCWSidebandMode();
			UiDriverShowMode();
			UiDriverUpdateFrequency(2,0);	// update frequency display without checking encoder, unconditionally updating synthesizer
		}
		//
		// disp_shift = 1;	// shift left to allow more room
		//
		break;
	//
	case MENU_TCXO_MODE:	// TCXO On/Off
		temp_sel = (df.temp_enabled & 0x0f);		// get current setting without upper nibble
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &temp_sel,
						0,
						TCXO_TEMP_STATE_MAX,
						TCXO_OFF,
						1
						);
		//
		if(lo.sensor_present)			// no sensor present
			temp_sel = TCXO_OFF;	// force TCXO disabled
		//
		df.temp_enabled = temp_sel | (df.temp_enabled & 0xf0);	// overlay new temperature setting with old status of upper nibble
		//
		if(temp_sel == TCXO_OFF)	{
			strcpy(options, " OFF");
			if(fchange)
				UiDriverCreateTemperatureDisplay(0,1);
		}
		else if(temp_sel == TCXO_ON)	{
			strcpy(options, "  ON");
			if(fchange)	{
				ui_si570_init_temp_sensor();
				UiDriverCreateTemperatureDisplay(1,1);
			}
		}
		else if(temp_sel == TCXO_STOP)	{
			strcpy(options, "STOP");
			if(fchange)
				UiDriverCreateTemperatureDisplay(0,1);
		}
		break;
		//
	case MENU_TCXO_C_F:	// TCXO display C/F mode
		if(df.temp_enabled & 0xf0)	// Yes - Is Fahrenheit mode enabled?
			temp_sel = 1;	// yes - set to 1
		else
			temp_sel = 0;	// no - Celsius
		//
		if((df.temp_enabled & 0x0f) != TCXO_STOP)	{	// is temperature display enabled at all?
			if(df.temp_enabled & 0xf0)	// Yes - Is Fahrenheit mode enabled?
				temp_sel = 1;	// yes - set to 1
			else
				temp_sel = 0;	// no - Celsius

			fchange = UiDriverMenuItemChangeUInt8(var, mode, &temp_sel,
							0,
							1,
							0,
							1
							);

			//
			if(temp_sel)					// Fahrenheit mode?
				df.temp_enabled |= 0xf0;	// set upper nybble
			else							// Celsius mode?
				df.temp_enabled &= 0x0f;	// clear upper nybble
		}
		else
			clr = Orange;
		//
		if(fchange)		// update screen if a change was made
			UiDriverCreateTemperatureDisplay(1,1);
		//
		if(temp_sel == 0)	{			// Celsius display
			strcpy(options, "C");
		}
		else if(temp_sel == 1)	{
			strcpy(options, "F");
		}
		break;
		//
	case MENU_SPEC_SCOPE_SPEED:	// spectrum scope speed
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.scope_speed,
						0,
						SPECTRUM_SCOPE_SPEED_MAX,
						SPECTRUM_SCOPE_SPEED_DEFAULT,
						1
						);
		//
		if(ts.scope_speed)
			sprintf(options, "  %u", ts.scope_speed);
		else
			strcpy(options, "OFF");
		break;
		//
	case MENU_SCOPE_FILTER_STRENGTH:	// spectrum filter strength
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.scope_filter,
						SPECTRUM_SCOPE_FILTER_MIN,
						SPECTRUM_SCOPE_FILTER_MAX,
						SPECTRUM_SCOPE_FILTER_DEFAULT,
						1
						);
		sprintf(options, "  %u", ts.scope_filter);
		break;
	//
	case MENU_SCOPE_TRACE_COLOUR:	// spectrum scope trace colour
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.scope_trace_colour,
						0,
						SPEC_ORANGE,
						SPEC_COLOUR_TRACE_DEFAULT,
						1
						);
		UiDriverMenuMapColors(ts.scope_trace_colour,options,&clr);
		break;
		//
	case MENU_SCOPE_GRID_COLOUR:	// spectrum scope grid colour
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.scope_grid_colour,
						0,
						SPEC_BLACK,
						SPEC_COLOUR_GRID_DEFAULT,
						1
						);
		UiDriverMenuMapColors(ts.scope_grid_colour,options,&clr);
		break;
		//
	case MENU_SCOPE_SCALE_COLOUR:	// spectrum scope/waterfall  scale colour
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.scope_scale_colour,
						0,
						SPEC_BLACK,
						SPEC_COLOUR_SCALE_DEFAULT,
						1
						);
		UiDriverMenuMapColors(ts.scope_scale_colour,options,&clr);
		// disp_shift = 1;
		break;
		//
	case MENU_SCOPE_MAGNIFY:	// Spectrum 2x magnify mode on/off
		UiDriverMenuItemChangeEnableOnOff(var, mode, &sd.magnify,0,options,&clr);
		break;
	case MENU_SCOPE_AGC_ADJUST:	// Spectrum scope AGC adjust
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.scope_agc_rate,
						SPECTRUM_SCOPE_AGC_MIN,
						SPECTRUM_SCOPE_AGC_MAX,
						SPECTRUM_SCOPE_AGC_DEFAULT,
						1
						);

		if(fchange)	{		// update system variable if rate changed
			sd.agc_rate = (float)ts.scope_agc_rate;	// calculate agc rate
			sd.agc_rate = sd.agc_rate/SPECTRUM_AGC_SCALING;
		}
		sprintf(options, "  %u", ts.scope_agc_rate);
		break;
	//
	case MENU_SCOPE_DB_DIVISION:	// Adjustment of dB/division of spectrum scope
		fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.spectrum_db_scale,
				DB_DIV_ADJUST_MIN,
				DB_DIV_ADJUST_MAX,
				DB_DIV_ADJUST_DEFAULT,
				1
		);
		switch(ts.spectrum_db_scale)	{	// convert variable to setting
			case DB_DIV_5:
				strcpy(options, "    5dB");
				break;
			case DB_DIV_7:
				strcpy(options, "  7.5dB");
				break;
			case DB_DIV_15:
				strcpy(options, "   15dB");
				break;
			case DB_DIV_20:
				strcpy(options, "   20dB");
				break;
			case S_1_DIV:
				strcpy(options, "1S-Unit");
				break;
			case S_2_DIV:
				strcpy(options, "2S-Unit");
				break;
			case S_3_DIV:
				strcpy(options, "3S-Unit");
				break;
			case DB_DIV_10:
			default:
				strcpy(options, "   10dB");
				break;
		}
		break;
	//
		case MENU_SCOPE_CENTER_LINE_COLOUR:	// spectrum scope grid center line colour
			fchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.scope_centre_grid_colour,
									0,
									SPEC_GREY2,
									SPEC_COLOUR_GRID_DEFAULT,
									1
									);
			//
			UiDriverMenuMapColors(ts.scope_centre_grid_colour,options,&clr);
						break;
			//
		case MENU_SCOPE_MODE:
			temp_sel = (ts.misc_flags1 & 0x80)?1:0;

			UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_sel,0,options,&clr);

			if (temp_sel) { ts.misc_flags1 |= 0x80; }
			else { ts.misc_flags1 &= ~0x80 ; }


			if(ts.misc_flags1 & 0x80)				// is waterfall mode active?
				strcpy(options, "WFALL");		// yes - indicate waterfall mode
			else
				strcpy(options, "SCOPE");		// no, scope mode

			break;
			//
		case MENU_WFALL_COLOR_SCHEME:	// Adjustment of dB/division of spectrum scope
			UiDriverMenuItemChangeUInt8(var, mode, &ts.waterfall_color_scheme,
					WATERFALL_COLOR_MIN,
					WATERFALL_COLOR_MAX,
					WATERFALL_COLOR_DEFAULT,
					1
					);
			switch(ts.waterfall_color_scheme)	{	// convert variable to setting
				case WFALL_HOT_COLD:
					strcpy(options, "HotCold");
					break;
				case WFALL_RAINBOW:
					strcpy(options, "Rainbow");
					break;
				case WFALL_BLUE:
					strcpy(options, "   Blue");
					break;
				case WFALL_GRAY_INVERSE:
					strcpy(options, "INVGrey");
					break;
				case WFALL_GRAY:
				default:
					strcpy(options, "   Grey" );
					break;
			}
			//
			break;
		//
		//
		case MENU_WFALL_STEP_SIZE:	// set step size of of waterfall display?
			UiDriverMenuItemChangeUInt8(var, mode, &ts.waterfall_vert_step_size,
					WATERFALL_STEP_SIZE_MIN,
					WATERFALL_STEP_SIZE_MAX,
					WATERFALL_STEP_SIZE_DEFAULT,
					1
					);
			sprintf(options, "  %u", ts.waterfall_vert_step_size);
			break;
			//
		case MENU_WFALL_OFFSET:	// set step size of of waterfall display?
			UiDriverMenuItemChangeUInt32(var, mode, &ts.waterfall_offset,
					WATERFALL_OFFSET_MIN,
					WATERFALL_OFFSET_MAX,
					WATERFALL_OFFSET_DEFAULT,
					1
					);
			sprintf(options, "  %u", (unsigned int)ts.waterfall_offset);
			break;
			//
		case MENU_WFALL_CONTRAST:	// set step size of of waterfall display?
			UiDriverMenuItemChangeUInt32(var, mode, &ts.waterfall_contrast,
					WATERFALL_CONTRAST_MIN,
					WATERFALL_CONTRAST_MAX,
					WATERFALL_CONTRAST_DEFAULT,
					2
					);
			sprintf(options, "  %u", (unsigned int)ts.waterfall_contrast);
			break;
		//
		case MENU_WFALL_SPEED:	// set step size of of waterfall display?
			UiDriverMenuItemChangeUInt8(var, mode, &ts.waterfall_speed,
					WATERFALL_SPEED_MIN,
					WATERFALL_SPEED_MAX,
					ts.display_type!=DISPLAY_HY28B_PARALLEL?WATERFALL_SPEED_DEFAULT_SPI:WATERFALL_SPEED_DEFAULT_PARALLEL,
					1
					);
			//
			if(ts.display_type != DISPLAY_HY28B_PARALLEL)	{
				if(ts.waterfall_speed <= WATERFALL_SPEED_WARN_SPI)
					clr = Red;
				else if(ts.waterfall_speed <= WATERFALL_SPEED_WARN1_SPI)
					clr = Yellow;
			}
			else	{
				if(ts.waterfall_speed <= WATERFALL_SPEED_WARN_PARALLEL)
					clr = Red;
				else if(ts.waterfall_speed <= WATERFALL_SPEED_WARN1_PARALLEL)
					clr = Yellow;
			}

			sprintf(options, "  %u", ts.waterfall_speed);
			break;
		//
		case MENU_SCOPE_NOSIG_ADJUST:	// set step size of of waterfall display?
			UiDriverMenuItemChangeUInt8(var, mode, &ts.spectrum_scope_nosig_adjust,
					SPECTRUM_SCOPE_NOSIG_ADJUST_MIN,
					SPECTRUM_SCOPE_NOSIG_ADJUST_MAX,
					SPECTRUM_SCOPE_NOSIG_ADJUST_DEFAULT,
					1
					);
			sprintf(options, "  %u", ts.spectrum_scope_nosig_adjust);
			break;
			//
		case MENU_WFALL_NOSIG_ADJUST:	// set step size of of waterfall display?
			UiDriverMenuItemChangeUInt8(var, mode, &ts.waterfall_nosig_adjust,
					WATERFALL_NOSIG_ADJUST_MIN,
					WATERFALL_NOSIG_ADJUST_MAX,
					WATERFALL_NOSIG_ADJUST_DEFAULT,
					1
					);
			sprintf(options, "  %u", ts.waterfall_nosig_adjust);
			break;
		//
		case MENU_WFALL_SIZE:	// set step size of of waterfall display?
			UiDriverMenuItemChangeUInt8(var, mode, &ts.waterfall_size,
						0,
						WATERFALL_MAX,
						WATERFALL_SIZE_DEFAULT,
						1
						);
			//
			switch(ts.waterfall_size)	{
				case WATERFALL_MEDIUM:
					strcpy(options, "Medium");
					break;
				case WATERFALL_NORMAL:
				default:
					strcpy(options, "Normal");
					break;
			}
			break;
	case MENU_BACKUP_CONFIG:
			strcpy(options," ");
			if(ts.ser_eeprom_in_use == 0)
			    {
			    strcpy(options, " Do it!");
			    clr = White;
			    if(var>=1)
				{
				UiLcdHy28_PrintText(POS_MENU_IND_X+189, POS_MENU_IND_Y+opt_pos*12,"Working",Red,Black,0);
				copy_ser2virt();
				strcpy(options, " Done...");
				clr = Green;
				UiDriverUpdateMenu(2);
				}
			    break;
			    }
			select = MENU_HARDWARE_INFO;
			break;
	case MENU_RESTORE_CONFIG:
			strcpy(options," ");
			if(ts.ser_eeprom_in_use == 0)
			    {
			    strcpy(options, "Do it!");
			    clr = White;
			    if(var>=1)
				{
				UiLcdHy28_PrintText(POS_MENU_IND_X+189, POS_MENU_IND_Y+opt_pos*12,"Working",Red,Black,0);
				copy_virt2ser();
				ui_si570_get_configuration();		// restore SI570 to factory default
				*(__IO uint32_t*)(SRAM2_BASE) = 0x55;
				NVIC_SystemReset();			// restart mcHF
				}
			    break;
			    }
			select = MENU_WFALL_SIZE;
			break;
	case MENU_HARDWARE_INFO:
			strcpy(options, "SHOW");
			clr = White;
			if(var>=1)
			    {
			    strcpy(options, " ");
			    clr = White;
			    UiDriverUpdateMenu(9);
			    }
			break;
	case MENU_DUMMY_LINE_2:
			strcpy(options, " ");
			break;
	case MENU_DUMMY_LINE_3:
			strcpy(options, " ");
			break;
	case MENU_DUMMY_LINE_4:
			strcpy(options, " ");
			break;
	case MENU_DUMMY_LINE_5:
			strcpy(options, " ");
			break;
//
//
// ******************  Make sure that this menu item is ALWAYS the last of the main menu items!
//
	case MENU_CONFIG_ENABLE:	// Radio Config Menu Enable - not saved in EEPROM, does not trigger "save" indicator
		if(var >= 1)	{	// setting increase?
			ts.radio_config_menu_enable = 1;
		}
		else if(var <= -1)	{	// setting decrease?
			ts.radio_config_menu_enable = 0;
		}
		//
		if(mode == 3)
			ts.radio_config_menu_enable = 0;
		//
		if(ts.radio_config_menu_enable)	{
			strcpy(options, " ON");
			clr = Orange;
		}
		else
			strcpy(options, "OFF");
		break;
	default:						// Move to this location if we get to the bottom of the table!
		strcpy(options, "ERROR!");
		break;
	}
	//
	UiLcdHy28_PrintTextRight(POS_MENU_CURSOR_X - 4, POS_MENU_IND_Y + (opt_pos * 12), options, clr, Black, 0);		// yes, normal position
	if(mode == 1)	{
		if(opt_oldpos != 999)		// was the position of a previous cursor stored?
			UiLcdHy28_PrintText(POS_MENU_CURSOR_X, POS_MENU_IND_Y + (opt_oldpos * 12), " ", Black, Black, 0);	// yes - erase it
		//
		opt_oldpos = opt_pos;	// save position of new "old" cursor position
		UiLcdHy28_PrintText(POS_MENU_CURSOR_X, POS_MENU_IND_Y + (opt_pos * 12), "<", Green, Black, 0);	// place cursor at active position
	}
	//
	return;
}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//

//
//*----------------------------------------------------------------------------
//* Function Name       : UiDriverUpdateConfigMenuLines
//* Object              : Display and and change line items related to the radio hardware configuration
//* Input Parameters    : index:  Line to display  mode:  0=display/update 1=change item 3=set default
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
//
static void UiDriverUpdateConfigMenuLines(uchar index, uchar mode)
{
	char options[32];
	ulong opt_pos;					// y position of option
	static ulong opt_oldpos = 999;	// y position of option
	uchar select;
	ulong	clr;
	ulong	calc_var;
	uchar	temp_var;
	int var;
	bool tchange = 0;		// used to indicate a parameter change
	// bool disp_shift = 0;	// used to cause display to be shifted to the left for large amounts of data (e.g. frequency displays)
	opt_pos = 5;		// default in case of use with wrong index/mode values
	// float ftemp;

	clr = White;		// color used it display of adjusted options

	if(mode == 0)	{	// are we in update/display mode?
		select = index;	// use index passed from calling function
		var = 0;		// prevent any change of variable
	}
	else	{			// this is "change" mode
		select = ts.menu_item-MAX_MENU_ITEM;	// item selected from encoder
		var = ts.menu_var;		// change from encoder
		ts.menu_var = 0;		// clear encoder change detect
	}
	strcpy(options, "ERROR");	// pre-load to catch error condition
	//
	if(mode == 1)	{
		if(select == CONFIG_FREQUENCY_CALIBRATE)	// signal if we are in FREQUENCY CALIBRATE mode for alternate frequency steps
			ts.freq_cal_adjust_flag = 1;
		else	{							// NOT in frequency calibrate mode
			if(ts.freq_cal_adjust_flag)	{	// had frequency calibrate mode been active?
				ts.freq_cal_adjust_flag = 0;
				UiDriverChangeTuningStep(0);	// force to valid frequency step size for normal tuning
				UiDriverChangeTuningStep(1);
			}
		}
		//
		if(select == CONFIG_XVTR_FREQUENCY_OFFSET)	// signal if we are in XVTR FREQUENCY OFFSET adjust mode for alternate frequency steps
			ts.xvtr_adjust_flag = 1;
		else	{							// NOT in transverter mode
			if(ts.xvtr_adjust_flag)	{		// had transverter frequency mode been active?
				ts.xvtr_adjust_flag = 0;	// yes - disable flag
				UiDriverChangeTuningStep(0);	// force to valid frequency step size for normal tuning
				UiDriverChangeTuningStep(1);
			}
		}
	}
	//

	opt_pos = select % MENUSIZE;
	switch(select)	{		//
	//
	case CONFIG_FREQ_STEP_MARKER_LINE:	// Frequency step marker line on/off
		temp_var = ts.freq_step_config & 0x0f;
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		if(tchange)	{		// something changed?
			if(temp_var)		// yes, is line to be enabled?
				ts.freq_step_config |= 0x0f;	// yes, set lower nybble
			else			// line disabled?
				ts.freq_step_config &= 0xf0;	// no, clear lower nybble
			//
			UiDriverShowStep(df.tuning_step);	// update screen
		}
		break;
	case CONFIG_STEP_SIZE_BUTTON_SWAP:	// Step size button swap on/off
		temp_var = ts.freq_step_config & 0xf0;
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		if(tchange)	{
			if(temp_var)	// is button to be swapped?
				ts.freq_step_config |= 0xf0;	// set upper nybble
			else			// line disabled?
				ts.freq_step_config &= 0x0f;	// clear upper nybble
		}
		break;
	case CONFIG_BAND_BUTTON_SWAP:	// Swap position of Band+ and Band- buttons
		temp_var = ts.misc_flags1 & 2;
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		if(tchange)	{
			if(temp_var)	// band up/down swap is to be enabled
				ts.misc_flags1 |= 2;		// set LSB
			else			// band up/down swap is to be disabled
				ts.misc_flags1 &= 0xfd;		// clear LSB
		}
		break;
	case CONFIG_TX_DISABLE:	// Step size button swap on/off
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &ts.tx_disable,0,options,&clr);
		if(ts.tx_disable)	{			// Transmit disabled?
			UiLcdHy28_PrintText(POS_BOTTOM_BAR_F5_X,POS_BOTTOM_BAR_F5_Y,"  TUNE",Grey1,Black,0);	// Make TUNE button Grey
		}
		else	{
			UiLcdHy28_PrintText(POS_BOTTOM_BAR_F5_X,POS_BOTTOM_BAR_F5_Y,"  TUNE",White,Black,0);	// Make TUNE button White
		}
		break;
	case CONFIG_AUDIO_MAIN_SCREEN_MENU_SWITCH:	// AFG/(STG/CMP) and RIT/(WPM/MIC/LIN) are to change automatically with TX/RX
		temp_var = ts.misc_flags1 & 1;
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		if(tchange)	{
			if(temp_var)	// change-on-tx is to be disabled
				ts.misc_flags1 |= 1;		// set LSB
			else			// change-on-tx is to be enabled
				ts.misc_flags1 &= 0xfe;		// clear LSB
		}
		break;
	case CONFIG_MUTE_LINE_OUT_TX:	// Enable/disable MUTE of TX audio on LINE OUT
		temp_var = ts.misc_flags1 & 4;
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		if((tchange) && (!ts.iq_freq_mode))		{	// did the status change and is translate mode NOT active?
			if(temp_var)	// Yes - MUTE of TX audio on LINE OUT is enabled
				ts.misc_flags1 |= 4;		// set LSB
			else			// MUTE of TX audio on LINE OUT is disabled
				ts.misc_flags1 &= 0xfb;		// clear LSB
		}
		if(ts.iq_freq_mode)	// Mark RED if translate mode is active
			clr = Red;
		break;
	case CONFIG_TX_AUDIO_MUTE:	// maximum RX gain setting
		UiDriverMenuItemChangeUInt8(var, mode, &ts.tx_audio_muting_timing,
				0,
				TX_PTT_AUDIO_MUTE_DELAY_MAX,
				0,
				1
				);
		sprintf(options, "    %u", ts.tx_audio_muting_timing);
		break;
	case CONFIG_LCD_AUTO_OFF_MODE:	// LCD auto-off mode control
		temp_var = ts.lcd_backlight_blanking;		// get control variable
		temp_var &= 0x0f;							// mask off upper nybble
		tchange = UiDriverMenuItemChangeUInt8(var, mode, &temp_var,
				0,
				0x0f,
				BACKLIGHT_BLANK_TIMING_DEFAULT,
				1
				);
		if(tchange)			{					// timing has been changed manually
			if(temp_var)	{				// is the time non-zero?
				ts.lcd_backlight_blanking = temp_var;	// yes, copy current value into variable
				ts.lcd_backlight_blanking |= 0x80;		// set MSB to enable auto-blanking
			}
			else {
				ts.lcd_backlight_blanking = 0;			// zero out variable
			}
			UiLCDBlankTiming();		// update the LCD timing parameters
		}
		//
		if(ts.lcd_backlight_blanking & 0x80)			// timed auto-blanking enabled?
			sprintf(options,"%02d sec",ts.lcd_backlight_blanking & 0x0f);	// yes - Update screen indicator with number of seconds
		else
			sprintf(options,"   OFF");						// Or if turned off
		break;
	case CONFIG_VOLTMETER_CALIBRATION:		// Voltmeter calibration
		tchange = UiDriverMenuItemChangeUInt32(var, mode, &ts.voltmeter_calibrate,
				POWER_VOLTMETER_CALIBRATE_MIN,
				POWER_VOLTMETER_CALIBRATE_MAX,
				POWER_VOLTMETER_CALIBRATE_DEFAULT,
				1
				);
		sprintf(options, "  %u", (unsigned int)ts.voltmeter_calibrate);
		break;
	case CONFIG_DISP_FILTER_BANDWIDTH: // Display filter bandwidth
		tchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.filter_disp_colour,
				0,
				SPEC_BLACK,
				SPEC_COLOUR_GRID_DEFAULT,
				1
				);
		UiDriverMenuMapColors(ts.filter_disp_colour,options,&clr);
		break;
		//
	case CONFIG_MAX_VOLUME:	// maximum audio volume
		UiDriverMenuItemChangeUInt8(var, mode, &ts.rx_gain[RX_AUDIO_SPKR].max,
						MAX_VOLUME_MIN,
						MAX_VOLUME_MAX,
						MAX_VOLUME_DEFAULT,
						1
						);
		if(ts.rx_gain[RX_AUDIO_SPKR].value > ts.rx_gain[RX_AUDIO_SPKR].max)	{			// is the volume currently higher than the new setting?
			ts.rx_gain[RX_AUDIO_SPKR].value = ts.rx_gain[RX_AUDIO_SPKR].max;		// yes - force the volume to the new value
			UiDriverChangeAfGain(0);
		}
		sprintf(options, "    %u", ts.rx_gain[RX_AUDIO_SPKR].value);
		//
		if(ts.rx_gain[RX_AUDIO_SPKR].max <= MAX_VOL_RED_THRESH)			// Indicate that gain has been reduced by changing color
			clr = Red;
		else if(ts.rx_gain[RX_AUDIO_SPKR].max <= MAX_VOLT_YELLOW_THRESH)
			clr = Orange;
		break;
	case CONFIG_MAX_RX_GAIN:	// maximum RX gain setting
		tchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.max_rf_gain,
						0,
						MAX_RF_GAIN_MAX,
						MAX_RF_GAIN_DEFAULT,
						1
						);
		if(tchange)	{
			AudioManagement_CalcAGCVals();	// calculate new internal AGC values from user settings
		}
		sprintf(options, "    %u", ts.max_rf_gain);
		break;
	case CONFIG_BEEP_ENABLE:	//
		temp_var = ts.misc_flags2 & 4;
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		if(tchange)	{
			if(temp_var)	// beep is to be enabled
				ts.misc_flags2 |= 4;		// set LSB+2
			else			// beep is to be disabled
				ts.misc_flags2 &= 0xfb;		// clear LSB+2
			UiDriverUpdateMenu(0);
		}
		break;
	case CONFIG_BEEP_FREQ:		// Beep frequency
		if(ts.misc_flags2 & 4)	{	// is beep enabled?
			tchange = UiDriverMenuItemChangeUInt32(var, mode, &ts.beep_frequency,
								MIN_BEEP_FREQUENCY,
								MAX_BEEP_FREQUENCY,
								DEFAULT_BEEP_FREQUENCY,
								25);
			if(tchange)		{
				AudioManagement_LoadBeepFreq();
				AudioManagement_KeyBeep();		// make beep to demonstrate frequency
			}
		}
		else	// beep not enabled - display frequency in red
			clr = Orange;
		sprintf(options, "   %uHz", (uint)ts.beep_frequency);	// casted to int because display errors if uint32_t
		break;
	//
	case CONFIG_BEEP_LOUDNESS:	// beep loudness
		tchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.beep_loudness,
							0,
							MAX_BEEP_LOUDNESS,
							DEFAULT_BEEP_LOUDNESS,
							1);
		if(tchange)	{
			AudioManagement_LoadBeepFreq();	// calculate new beep loudness values
			AudioManagement_KeyBeep();		// make beep to demonstrate loudness
		}
		sprintf(options, "    %u", ts.beep_loudness);
		break;
	//
	//
	// *****************  WARNING *********************
	// If you change CAT mode, THINGS MAY GET "BROKEN" - for example, you may not be able to reliably save to EEPROM!
	// This needs to be investigated!
	//
	case CONFIG_CAT_ENABLE:	// CAT mode	 - not saved in EEPROM, does not trigger "save" indicator
		temp_var = ts.cat_mode_active;
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		ts.cat_mode_active = temp_var;
		if (tchange) {
			if(ts.cat_mode_active)	{
				cat_driver_init();
			}
			else	{
				cat_driver_stop();
			}
		}
		break;
	case CONFIG_FREQUENCY_CALIBRATE:		// Frequency Calibration
		if(var >= 1)	{	// setting increase?
			ts.menu_var_changed = 1;	// indicate that a change has occurred
			ts.freq_cal += df.tuning_step;
			tchange = 1;
		}
		else if(var <= -1)	{	// setting decrease?
			ts.menu_var_changed = 1;	// indicate that a change has occurred
			ts.freq_cal -= df.tuning_step;
			tchange = 1;
		}
		if(ts.freq_cal < MIN_FREQ_CAL)
			ts.freq_cal = MIN_FREQ_CAL;
		else if(ts.freq_cal > MAX_FREQ_CAL)
			ts.freq_cal = MAX_FREQ_CAL;
		//
		if(mode == 3)	{
			ts.menu_var_changed = 1;	// indicate that a change has occurred
			ts.freq_cal = 0;
			tchange = 1;
		}
		if(tchange)
			UiDriverUpdateFrequency(2,0);	// Update LO frequency without checking encoder but overriding "frequency didn't change" detect
		// disp_shift = 1;
		sprintf(options, "   %d", ts.freq_cal);
		break;
		//
	case CONFIG_FREQ_LIMIT_RELAX:	// Enable/disable Frequency tuning limits
		temp_var = ts.misc_flags1 & 32;
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		if(tchange)		{	// did the status change and is translate mode NOT active?
			if(temp_var)	// tuning limit is disabled
				ts.misc_flags1 |= 32;		// set bit
			else			// tuning limit is enabled
				ts.misc_flags1 &= 0xdf;		// clear bit
		}
		if(ts.misc_flags1 & 32)	{			// tuning limit is disabled
			clr = Orange;					// warn user!
		}
		break;
	case CONFIG_FREQ_MEM_LIMIT_RELAX:	// Enable/disable Frequency memory limits
		temp_var = ts.misc_flags2 & 16;
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		if(tchange)		{	// did the status change?
			if(temp_var)	// freq/mem limit is disabled
				ts.misc_flags2 |= 16;		// set bit
			else			// freq/mem limit is enabled
				ts.misc_flags2 &= 0xef;		// clear bit
		}
		if(ts.misc_flags2 & 16)	{			// frequency/memory limit is disabled
			clr = Orange;					// warn user!
		}
		break;
	case CONFIG_LSB_RX_IQ_GAIN_BAL:		// LSB RX IQ Gain balance
		if((ts.dmod_mode == DEMOD_LSB) && (ts.txrx_mode == TRX_MODE_RX)) 	{	// only allow adjustment if in LSB mode
			tchange = UiDriverMenuItemChangeInt(var, mode, &ts.rx_iq_lsb_gain_balance,
								MIN_RX_IQ_GAIN_BALANCE,
								MAX_RX_IQ_GAIN_BALANCE,
								0,
								1);
			if(tchange)
				AudioManagement_CalcRxIqGainAdj();
		}
		else		// Orange if not in RX and/or correct mode
			clr = Orange;
		sprintf(options, "   %d", ts.rx_iq_lsb_gain_balance);
		break;
	case CONFIG_LSB_RX_IQ_PHASE_BAL:		// LSB RX IQ Phase balance
		if((ts.dmod_mode == DEMOD_LSB) && (ts.txrx_mode == TRX_MODE_RX))	{
			tchange = UiDriverMenuItemChangeInt(var, mode, &ts.rx_iq_lsb_phase_balance,
								MIN_RX_IQ_PHASE_BALANCE,
								MAX_RX_IQ_PHASE_BALANCE,
								0,
								1);
			if(tchange)
				AudioFilter_CalcRxPhaseAdj();
		}
		else		// Orange if not in RX and/or correct mode
			clr = Orange;
		sprintf(options, "   %d", ts.rx_iq_lsb_phase_balance);
		break;
	case CONFIG_USB_RX_IQ_GAIN_BAL:		// USB/CW RX IQ Gain balance
		if(((ts.dmod_mode == DEMOD_USB) || (ts.dmod_mode == DEMOD_CW))  && (ts.txrx_mode == TRX_MODE_RX))	{
			tchange = UiDriverMenuItemChangeInt(var, mode, &ts.rx_iq_usb_gain_balance,
								MIN_RX_IQ_GAIN_BALANCE,
								MAX_RX_IQ_GAIN_BALANCE,
								0,
								1);
			if(tchange)
				AudioManagement_CalcRxIqGainAdj();
		}
		else		// Orange if not in RX and/or correct mode
			clr = Orange;
		sprintf(options, "   %d", ts.rx_iq_usb_gain_balance);
		break;
	case CONFIG_USB_RX_IQ_PHASE_BAL:		// USB RX IQ Phase balance
		if((ts.dmod_mode == DEMOD_USB)  && (ts.txrx_mode == TRX_MODE_RX))	{
			tchange = UiDriverMenuItemChangeInt(var, mode, &ts.rx_iq_usb_phase_balance,
											MIN_RX_IQ_PHASE_BALANCE,
											MAX_RX_IQ_PHASE_BALANCE,
											0,
											1);if(tchange)
				AudioFilter_CalcRxPhaseAdj();
		}
		else		// Orange if not in RX and/or correct mode
			clr = Orange;
		sprintf(options, "   %d", ts.rx_iq_usb_phase_balance);
		break;
	case 	CONFIG_AM_RX_GAIN_BAL:		// AM RX IQ Phase balance
		if((ts.dmod_mode == DEMOD_AM)  && (ts.txrx_mode == TRX_MODE_RX))	{
			tchange = UiDriverMenuItemChangeInt(var, mode, &ts.rx_iq_am_gain_balance,
											MIN_RX_IQ_GAIN_BALANCE,
											MAX_RX_IQ_GAIN_BALANCE,
											0,
											1);
			if(tchange)
				AudioManagement_CalcRxIqGainAdj();
		}
		else		// Orange if not in RX and/or correct mode
			clr = Orange;
		sprintf(options, "   %d", ts.rx_iq_am_gain_balance);
		break;
	case 	CONFIG_FM_RX_GAIN_BAL:		// FM RX IQ Phase balance
		if((ts.dmod_mode == DEMOD_FM)  && (ts.txrx_mode == TRX_MODE_RX))	{
			tchange = UiDriverMenuItemChangeInt(var, mode, &ts.rx_iq_fm_gain_balance,
													MIN_RX_IQ_GAIN_BALANCE,
													MAX_RX_IQ_GAIN_BALANCE,
													0,
													1);if(tchange)
				AudioManagement_CalcRxIqGainAdj();
		}
		else		// Orange if not in RX and/or correct mode
			clr = Orange;
		sprintf(options, "   %d", ts.rx_iq_fm_gain_balance);
		break;
	case CONFIG_LSB_TX_IQ_GAIN_BAL:		// LSB TX IQ Gain balance
		if((ts.dmod_mode == DEMOD_LSB) && (ts.txrx_mode == TRX_MODE_TX))	{
			tchange = UiDriverMenuItemChangeInt(var, mode, &ts.tx_iq_lsb_gain_balance,
													MIN_TX_IQ_GAIN_BALANCE,
													MAX_TX_IQ_GAIN_BALANCE,
													0,
													1);
			if(tchange)
				AudioManagement_CalcTxIqGainAdj();
		}
		else		// Orange if not in TX and/or correct mode
			clr = Orange;
		sprintf(options, "   %d", ts.tx_iq_lsb_gain_balance);
		break;
	case CONFIG_LSB_TX_IQ_PHASE_BAL:		// LSB TX IQ Phase balance
		if((ts.dmod_mode == DEMOD_LSB) && (ts.txrx_mode == TRX_MODE_TX))	{
			tchange = UiDriverMenuItemChangeInt(var, mode, &ts.tx_iq_lsb_phase_balance,
					MIN_TX_IQ_PHASE_BALANCE,
					MAX_TX_IQ_PHASE_BALANCE,
					0,
					1);
			if(tchange)
				AudioFilter_CalcTxPhaseAdj();
		}
		else		// Orange if not in TX and/or correct mode
			clr = Orange;
		sprintf(options, "   %d", ts.tx_iq_lsb_phase_balance);
		break;
	case CONFIG_USB_TX_IQ_GAIN_BAL:		// USB/CW TX IQ Gain balance
		if(((ts.dmod_mode == DEMOD_USB) || (ts.dmod_mode == DEMOD_CW)) && (ts.txrx_mode == TRX_MODE_TX))	{
			tchange = UiDriverMenuItemChangeInt(var, mode, &ts.tx_iq_usb_gain_balance,
													MIN_TX_IQ_GAIN_BALANCE,
													MAX_TX_IQ_GAIN_BALANCE,
													0,
													1);
			if(tchange)
				AudioManagement_CalcTxIqGainAdj();
		}
		else		// Orange if not in TX and/or correct mode
			clr = Orange;
		sprintf(options, "   %d", ts.tx_iq_usb_gain_balance);
		break;
	case CONFIG_USB_TX_IQ_PHASE_BAL:		// USB TX IQ Phase balance
		if((ts.dmod_mode == DEMOD_USB) && (ts.txrx_mode == TRX_MODE_TX))	{
			tchange = UiDriverMenuItemChangeInt(var, mode, &ts.tx_iq_usb_phase_balance,
					MIN_TX_IQ_PHASE_BALANCE,
					MAX_TX_IQ_PHASE_BALANCE,
					0,
					1);
			if(tchange)
				AudioFilter_CalcTxPhaseAdj();
		}
		else		// Orange if not in TX and/or correct mode
			clr = Orange;
		//
		sprintf(options, "   %d", ts.tx_iq_usb_phase_balance);
		opt_pos = CONFIG_USB_TX_IQ_PHASE_BAL % MENUSIZE;
		break;
		//
	case 	CONFIG_AM_TX_GAIN_BAL:		// AM TX IQ Phase balance
		if((ts.dmod_mode == DEMOD_AM) && (ts.txrx_mode == TRX_MODE_TX))	{
			tchange = UiDriverMenuItemChangeInt(var, mode, &ts.tx_iq_am_gain_balance,
					MIN_TX_IQ_GAIN_BALANCE,
					MAX_TX_IQ_GAIN_BALANCE,
					0,
					1);
			if(tchange)
				AudioManagement_CalcTxIqGainAdj();
		}
		else		// Orange if not in TX and/or correct mode
			clr = Orange;
		sprintf(options, "   %d", ts.tx_iq_am_gain_balance);
		break;
	case 	CONFIG_FM_TX_GAIN_BAL:		// FM TX IQ Phase balance
		if((ts.dmod_mode == DEMOD_FM)  && (ts.txrx_mode == TRX_MODE_TX))	{
			tchange = UiDriverMenuItemChangeInt(var, mode, &ts.tx_iq_fm_gain_balance,
					MIN_TX_IQ_GAIN_BALANCE,
					MAX_TX_IQ_GAIN_BALANCE,
					0,
					1);
			if(tchange)
				AudioManagement_CalcTxIqGainAdj();
		}
		else		// Orange if not in TX and/or correct mode
			clr = Orange;
		sprintf(options, "   %d", ts.tx_iq_fm_gain_balance);
		break;
	case CONFIG_CW_PA_BIAS:		// CW PA Bias adjust
		if((ts.tune) || (ts.txrx_mode == TRX_MODE_TX))	{	// enable only in TUNE mode
			tchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.pa_cw_bias,
					0,
					MAX_PA_BIAS,
					0,
					1);

			if(tchange)	{
				if((ts.dmod_mode == DEMOD_CW) && (ts.pa_cw_bias))	{	// in CW mode and bias NONZERO?
					calc_var = BIAS_OFFSET + (ts.pa_cw_bias * 2);
					if(calc_var > 255)
						calc_var = 255;
					//
					DAC_SetChannel2Data(DAC_Align_8b_R,calc_var);	// Set DAC Channel 1 DHR12L register
				}
				else	{
					calc_var = BIAS_OFFSET + (ts.pa_bias * 2);	// if it is zero, use the "other" value
					if(calc_var > 255)
						calc_var = 255;
					//
					DAC_SetChannel2Data(DAC_Align_8b_R,calc_var);	// Set DAC Channel 1 DHR12L register
				}
			}
			if(ts.pa_cw_bias < MIN_BIAS_SETTING)
				clr = Yellow;
		}
		else		// Orange if not in TUNE or TX mode
			clr = Orange;
		sprintf(options, "  %u", ts.pa_cw_bias);
		break;
	case CONFIG_PA_BIAS:		// PA Bias adjust (Including CW if CW bias == 0)
		if((ts.tune) || (ts.txrx_mode == TRX_MODE_TX))	{	// enable only in TUNE mode
			tchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.pa_bias,
					0,
					MAX_PA_BIAS,
					0,
					1);

			if(tchange)	{
				if(ts.dmod_mode != DEMOD_CW || ts.pa_cw_bias == 0)	{	// is it NOT in CW mode, or is it in CW mode and the CW bias set to zero?
					calc_var = BIAS_OFFSET + (ts.pa_bias * 2);
					if(calc_var > 255)
						calc_var = 255;
					//
					// Set DAC Channel 1 DHR12L register
					DAC_SetChannel2Data(DAC_Align_8b_R,calc_var);
				}
			}
			if(ts.pa_bias < MIN_BIAS_SETTING)
				clr = Yellow;
		}
		else		// Orange if not in TUNE or TX mode
			clr = Orange;
		sprintf(options, "  %u", ts.pa_bias);
		break;
	case CONFIG_FWD_REV_PWR_DISP:	// Enable/disable swap of FWD/REV A/D inputs on power sensor
		temp_var = swrm.pwr_meter_disp;
		UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		swrm.pwr_meter_disp = temp_var;
		break;
	case CONFIG_RF_FWD_PWR_NULL:		// RF power FWD power meter calibrate
		if(swrm.pwr_meter_disp)	{
			tchange = UiDriverMenuItemChangeUInt8(var, mode, &swrm.sensor_null,
					SWR_CAL_MIN,
					SWR_CAL_MAX,
					SWR_CAL_DEFAULT,
					1);
			if(ts.txrx_mode != TRX_MODE_TX)	// Orange if not in TX mode
					clr = Orange;
		}
		else	// numerical display NOT active
			clr = Orange;		// make it red to indicate that adjustment is NOT available
		sprintf(options, "  %u", swrm.sensor_null);
		break;
	case CONFIG_FWD_REV_COUPLING_2200M_ADJ:		// RF power sensor coupling adjust (2200m)
		UiDriverMenuBandRevCouplingAdjust(var, mode, FILTER_BAND_2200, &swrm.coupling_2200m_calc, options, &clr);
		break;
	case CONFIG_FWD_REV_COUPLING_630M_ADJ:		// RF power sensor coupling adjust (630m)
		UiDriverMenuBandRevCouplingAdjust(var, mode, FILTER_BAND_630, &swrm.coupling_630m_calc, options, &clr);
		break;
	case CONFIG_FWD_REV_COUPLING_160M_ADJ:		// RF power sensor coupling adjust (160m)
		UiDriverMenuBandRevCouplingAdjust(var, mode, FILTER_BAND_160, &swrm.coupling_160m_calc, options, &clr);
		break;
	case CONFIG_FWD_REV_COUPLING_80M_ADJ:		// RF power sensor coupling adjust (80m)
		UiDriverMenuBandRevCouplingAdjust(var, mode, FILTER_BAND_80, &swrm.coupling_80m_calc, options, &clr);
		break;
	case CONFIG_FWD_REV_COUPLING_40M_ADJ:		// RF power sensor coupling adjust (40m)
		UiDriverMenuBandRevCouplingAdjust(var, mode, FILTER_BAND_40, &swrm.coupling_40m_calc, options, &clr);
		break;
	case CONFIG_FWD_REV_COUPLING_20M_ADJ:		// RF power sensor coupling adjust (20m)
		UiDriverMenuBandRevCouplingAdjust(var, mode, FILTER_BAND_20, &swrm.coupling_20m_calc, options, &clr);
		break;
	case CONFIG_FWD_REV_COUPLING_15M_ADJ:		// RF power sensor coupling adjust (15m)
		UiDriverMenuBandRevCouplingAdjust(var, mode, FILTER_BAND_15, &swrm.coupling_15m_calc, options, &clr);
		break;
	case CONFIG_FWD_REV_COUPLING_6M_ADJ:		// RF power sensor coupling adjust (6m)
		UiDriverMenuBandRevCouplingAdjust(var, mode, FILTER_BAND_6, &swrm.coupling_6m_calc, options, &clr);
		break;
	case CONFIG_FWD_REV_COUPLING_2M_ADJ:		// RF power sensor coupling adjust (2m)
		UiDriverMenuBandRevCouplingAdjust(var, mode, FILTER_BAND_2, &swrm.coupling_2m_calc, options, &clr);
		break;
	case CONFIG_FWD_REV_COUPLING_70CM_ADJ:		// RF power sensor coupling adjust (70cm)
		UiDriverMenuBandRevCouplingAdjust(var, mode, FILTER_BAND_70, &swrm.coupling_70cm_calc, options, &clr);
		break;
	case CONFIG_FWD_REV_COUPLING_23CM_ADJ:		// RF power sensor coupling adjust (23cm)
		UiDriverMenuBandRevCouplingAdjust(var, mode, FILTER_BAND_23, &swrm.coupling_23cm_calc, options, &clr);
		break;
	case CONFIG_FWD_REV_SENSE_SWAP:	// Enable/disable swap of FWD/REV A/D inputs on power sensor
		temp_var = ts.misc_flags1 & 16;
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		if(tchange)		{	// did the status change and is translate mode NOT active?
			if(temp_var)	// swapping of FWD/REV is enabled
				ts.misc_flags1 |= 16;		// set bit
			else			// swapping of FWD/REV bit is disabled
				ts.misc_flags1 &= 0xef;		// clear bit
		}
		if(ts.misc_flags1 & 16)	{			// Display status FWD/REV swapping
			clr = Orange;					// warn user swapping is on!
		}
		break;
	case CONFIG_XVTR_OFFSET_MULT:	// Transverter Frequency Display Offset/Multiplier Mode On/Off
		tchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.xverter_mode,
				0,
				XVERTER_MULT_MAX,
				0,
				1);
		if(tchange)	{		// change?
			ts.refresh_freq_disp = 1;	// cause frequency display to be completely refreshed
			if(ts.vfo_mem_mode & 128)	{	// in SPLIT mode?
				UiDriverUpdateFrequency(1,2);	// update RX frequency
				UiDriverUpdateFrequency(1,3);	// force display of second (TX) VFO frequency
			}
			else	// not in SPLIT mode - standard update
				UiDriverUpdateFrequency(2,0);	// update frequency display without checking encoder, unconditionally updating synthesizer
			ts.refresh_freq_disp = 0;
		}
		//
		if(ts.xverter_mode)	{
			sprintf(options, " ON x%u", ts.xverter_mode);	// Display on/multiplication factor
			clr = Red;
		}
		else
			strcpy(options, "    OFF");
		break;
	case CONFIG_XVTR_FREQUENCY_OFFSET:		// Adjust transverter Frequency offset
		if(var >= 1)	{	// setting increase?
			ts.menu_var_changed = 1;	// indicate that a change has occurred
			ts.xverter_offset += df.tuning_step;
			tchange = 1;
		}
		else if(var <= -1)	{	// setting decrease?
			ts.menu_var_changed = 1;	// indicate that a change has occurred
			if(ts.xverter_offset >= df.tuning_step)	// subtract only if we have room to do so
				ts.xverter_offset -= df.tuning_step;
			else
				ts.xverter_offset = 0;				// else set to zero
			//
			tchange = 1;
		}
		//
		if(ts.xverter_offset > XVERTER_OFFSET_MAX)
			ts.xverter_offset  = XVERTER_OFFSET_MAX;
		//
		if(mode == 3)	{
			ts.menu_var_changed = 1;	// indicate that a change has occurred
			ts.xverter_offset = 0;		// default for this option is to zero it out
			tchange = 1;
		}
		//
		if(tchange)	{		// change?
			ts.refresh_freq_disp = 1;	// cause frequency display to be completely refreshed
			if(ts.vfo_mem_mode & 128)	{	// in SPLIT mode?
				UiDriverUpdateFrequency(1,2);	// update RX frequency
				UiDriverUpdateFrequency(1,3);	// force display of second (TX) VFO frequency
			}
			else	// not in SPLIT mode - standard update
				UiDriverUpdateFrequency(2,0);	// update frequency display without checking encoder, unconditionally updating synthesizer
			ts.refresh_freq_disp = 0;
			tchange = 1;
		}
		//
		if(ts.xverter_mode)	// transvert mode active?
			clr = Red;		// make number red to alert user of this!
		//
		// disp_shift = 1;		// cause display to be shifted to the left so that it will fit
		sprintf(options, " %9uHz", (uint)ts.xverter_offset);	// print with nine digits
		break;


	case CONFIG_2200M_5W_ADJUST:		// 2200m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_2200, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_630M_5W_ADJUST:		// 630m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_630, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_160M_5W_ADJUST:		// 160m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_160, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_80M_5W_ADJUST:		// 80m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_80, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_60M_5W_ADJUST:		// 60m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_60, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_40M_5W_ADJUST:		// 40m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_40, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_30M_5W_ADJUST:		// 30m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_30, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_20M_5W_ADJUST:		// 20m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_20, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_17M_5W_ADJUST:		// 17m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_17, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_15M_5W_ADJUST:		// 15m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_15, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_12M_5W_ADJUST:		// 12m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_12, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_10M_5W_ADJUST:		// 10m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_10, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_6M_5W_ADJUST:		// 6m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_6, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_4M_5W_ADJUST:		// 4m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_4, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_2M_5W_ADJUST:		// 2m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_2, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_70CM_5W_ADJUST:		// 70cm 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_70, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_23CM_5W_ADJUST:		// 23cm 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_23, PA_LEVEL_5W, options, &clr);
		break;
	case CONFIG_2200M_FULL_POWER_ADJUST:		// 2200m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_2200, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_630M_FULL_POWER_ADJUST:		// 630m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_630, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_160M_FULL_POWER_ADJUST:		// 160m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_160, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_80M_FULL_POWER_ADJUST:		// 80m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_80, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_60M_FULL_POWER_ADJUST:		// 60m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_60, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_40M_FULL_POWER_ADJUST:		// 40m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_40, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_30M_FULL_POWER_ADJUST:		// 30m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_30, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_20M_FULL_POWER_ADJUST:		// 20m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_20, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_17M_FULL_POWER_ADJUST:		// 17m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_17, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_15M_FULL_POWER_ADJUST:		// 15m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_15, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_12M_FULL_POWER_ADJUST:		// 12m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_12, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_10M_FULL_POWER_ADJUST:		// 10m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_10, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_6M_FULL_POWER_ADJUST:		// 6m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_6, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_4M_FULL_POWER_ADJUST:		// 4m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_4, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_2M_FULL_POWER_ADJUST:		// 2m 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_2, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_70CM_FULL_POWER_ADJUST:		// 70cm 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_70, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_23CM_FULL_POWER_ADJUST:		// 23cm 5 watt adjust
		UiDriverMenuBandPowerAdjust(var, mode, BAND_MODE_23, PA_LEVEL_FULL, options, &clr);
		break;
	case CONFIG_DSP_NR_DECORRELATOR_BUFFER_LENGTH:		// Adjustment of DSP noise reduction de-correlation delay buffer length
		ts.dsp_nr_delaybuf_len &= 0xfff0;	// mask bottom nybble to enforce 16-count boundary
		tchange = UiDriverMenuItemChangeUInt32(var, mode, &ts.dsp_nr_delaybuf_len,
				DSP_NR_BUFLEN_MIN,
				DSP_NR_BUFLEN_MAX,
				DSP_NR_BUFLEN_DEFAULT,
				16);

		if(ts.dsp_nr_delaybuf_len <= ts.dsp_nr_numtaps)	// is buffer smaller/equal to number of taps?
			ts.dsp_nr_delaybuf_len = ts.dsp_nr_numtaps + 16;	// yes - it must always be larger than number of taps!

		if(tchange)	{	// did something change?
			if(ts.dsp_active & 1)	// only update if DSP NR active
				audio_driver_set_rx_audio_filter();
		}
		if(!(ts.dsp_active & 1))	// mark orange if DSP NR not active
			clr = Orange;
		if(ts.dsp_nr_numtaps >= ts.dsp_nr_delaybuf_len)	// Warn if number of taps greater than/equal buffer length!
			clr = Red;
		sprintf(options, "  %u", (uint)ts.dsp_nr_delaybuf_len);
		break;
	case CONFIG_DSP_NR_FFT_NUMTAPS:		// Adjustment of DSP noise reduction de-correlation delay buffer length
		ts.dsp_nr_numtaps &= 0xf0;	// mask bottom nybble to enforce 16-count boundary
		tchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.dsp_nr_numtaps,
				DSP_NR_NUMTAPS_MIN,
				DSP_NR_NUMTAPS_MAX,
				DSP_NR_NUMTAPS_DEFAULT,
				16);
		if(ts.dsp_nr_numtaps >= ts.dsp_nr_delaybuf_len)	// is number of taps equal or greater than buffer length?
			ts.dsp_nr_delaybuf_len = ts.dsp_nr_numtaps + 16;	// yes - make buffer larger

		if(tchange)	{	// did something change?
			if(ts.dsp_active & 1)	// only update if DSP NR active
				audio_driver_set_rx_audio_filter();
		}

		if(!(ts.dsp_active & 1))	// mark orange if DSP NR not active
			clr = Orange;
		if(ts.dsp_nr_numtaps >= ts.dsp_nr_delaybuf_len)	// Warn if number of taps greater than/equal buffer length!
			clr = Red;
		sprintf(options, "  %u", ts.dsp_nr_numtaps);
		break;
	case CONFIG_DSP_NR_POST_AGC_SELECT:		// selection of location of DSP noise reduction - pre audio filter/AGC or post AGC/filter
		temp_var = ts.dsp_active & 0x02;
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		if(!(ts.dsp_active & 1))	// mark orange if DSP NR not active
			clr = Orange;

		if (temp_var) { ts.dsp_active |= 0x02; } else { ts.dsp_active &= 0xfd; }
		if(tchange)	{	// did something change?
			if(ts.dsp_active & 1)	// only update if DSP NR active
				audio_driver_set_rx_audio_filter();
		}
		break;
	case CONFIG_DSP_NOTCH_CONVERGE_RATE:		// Adjustment of DSP noise reduction de-correlation delay buffer length
		tchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.dsp_notch_mu,
				0,
				DSP_NOTCH_MU_MAX,
				DSP_NOTCH_MU_DEFAULT,
				1);

		if(tchange)	{	// did something change?
			if(ts.dsp_active & 4)	// only update if Notch DSP is active
				audio_driver_set_rx_audio_filter();
		}
		if(!(ts.dsp_active & 4))	// mark orange if Notch DSP not active
			clr = Orange;
		sprintf(options, "  %u", ts.dsp_notch_mu);
		break;
	case CONFIG_DSP_NOTCH_DECORRELATOR_BUFFER_LENGTH:		// Adjustment of DSP noise reduction de-correlation delay buffer length
		tchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.dsp_notch_delaybuf_len,
				DSP_NOTCH_BUFLEN_MIN,
				DSP_NOTCH_BUFLEN_MAX,
				DSP_NOTCH_DELAYBUF_DEFAULT,
				8);


		if(ts.dsp_notch_delaybuf_len <= ts.dsp_notch_numtaps) 		// did we try to decrease it smaller than FFT size?
			ts.dsp_notch_delaybuf_len = ts.dsp_notch_numtaps + 8;						// yes - limit it to previous size
		if(tchange)	{	// did something change?
			if(ts.dsp_active & 4)	// only update if DSP Notch active
				audio_driver_set_rx_audio_filter();
		}
		if(!(ts.dsp_active & 4))	// mark orange if DSP Notch not active
			clr = Orange;
		if(ts.dsp_notch_numtaps >= ts.dsp_notch_delaybuf_len)
			clr = Red;
		sprintf(options, "  %u", (uint)ts.dsp_notch_delaybuf_len);
		break;
	case CONFIG_DSP_NOTCH_FFT_NUMTAPS:		// Adjustment of DSP noise reduction de-correlation delay buffer length
		ts.dsp_notch_numtaps &= 0xf0;	// mask bottom nybble to enforce 16-count boundary
		tchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.dsp_notch_numtaps,
				0,
				DSP_NOTCH_NUMTAPS_MAX,
				DSP_NOTCH_NUMTAPS_DEFAULT,
				16);
		if(ts.dsp_notch_numtaps >= ts.dsp_notch_delaybuf_len)	// force buffer size to always be larger than number of taps
			ts.dsp_notch_delaybuf_len = ts.dsp_notch_numtaps + 8;
		if(tchange)	{	// did something change?
			if(ts.dsp_active & 4)	// only update if DSP NR active
				audio_driver_set_rx_audio_filter();
		}
		if(!(ts.dsp_active & 4))	// mark orange if DSP NR not active
			clr = Orange;
		if(ts.dsp_notch_numtaps >= ts.dsp_notch_delaybuf_len)	// Warn if number of taps greater than/equal buffer length!
			clr = Red;
		sprintf(options, "  %u", ts.dsp_notch_numtaps);
		break;
	case CONFIG_AGC_TIME_CONSTANT:		// Adjustment of Noise Blanker AGC Time Constant
		tchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.nb_agc_time_const,
				0,
				NB_MAX_AGC_SETTING,
				NB_AGC_DEFAULT,
				1);
		if(tchange)	{				// parameter changed?
			AudioManagement_CalcNB_AGC();	// yes - recalculate new values for Noise Blanker AGC
		}
		//
		sprintf(options, "  %u", ts.nb_agc_time_const);
		break;
	case CONFIG_AM_TX_FILTER_ENABLE:	// Enable/disable AM TX audio filter
		temp_var = ts.misc_flags1 & 8;
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		if(tchange)		{	// did the status change and is translate mode NOT active?
			if(temp_var)	// AM TX audio filter is disabled
				ts.misc_flags1 |= 8;		// set LSB
			else			// AM TX audio filter is enabled
				ts.misc_flags1 &= 0xf7;		// clear LSB
		}
		if(ts.misc_flags1 & 8)	{			// Display status of TX audio filter
			clr = Orange;					// warn user that filter is off!
		}
		break;
	case CONFIG_SSB_TX_FILTER_ENABLE:	// Enable/disable SSB TX audio filter
		temp_var = ts.misc_flags1 & 64;
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		if(tchange)		{	// did the status change and is translate mode NOT active?
			if(temp_var)	// SSB TX audio filter is disabled
				ts.misc_flags1 |= 64;		// set bit
			else			// SSB TX audio filter is enabled
				ts.misc_flags1 &= 0xbf;		// clear bit
		}
		if(ts.misc_flags1 & 64)	{			// Display status of TX audio filter
			clr = Red;					// warn user that filter is off!
		}
		break;
	case CONFIG_TUNE_POWER_LEVEL: // set power for antenne tuning
		tchange = UiDriverMenuItemChangeUInt8(var*(-1), mode, &ts.tune_power_level,
				0,
				PA_LEVEL_MAX_ENTRY,
				PA_LEVEL_MAX_ENTRY,
				1);
		switch(ts.tune_power_level)	{
		case PA_LEVEL_FULL:
			strcpy(options, "FULL POWER");
			break;
		case PA_LEVEL_5W:
			strcpy(options, "        5W");
			break;
		case PA_LEVEL_2W:
			strcpy(options, "        2W");
			break;
		case PA_LEVEL_1W:
			strcpy(options, "        1W");
			break;
		case PA_LEVEL_0_5W:
			strcpy(options, "      0.5W");
			break;
		case PA_LEVEL_MAX_ENTRY:
			strcpy(options, " as TX PWR");
			break;
		}
		break;
	case CONFIG_FFT_WINDOW_TYPE:	// set step size of of waterfall display?
		tchange = UiDriverMenuItemChangeUInt8(var, mode, &ts.fft_window_type,
				0,
				FFT_WINDOW_MAX,
				FFT_WINDOW_DEFAULT,
				1);

		// disp_shift = 1;
		switch(ts.fft_window_type)	{
		case FFT_WINDOW_RECTANGULAR:
			strcpy(options, "Rectangular");
			break;
		case FFT_WINDOW_COSINE:
			strcpy(options, "     Cosine");
			break;
		case FFT_WINDOW_BARTLETT:
			strcpy(options, "   Bartlett");
			break;
		case FFT_WINDOW_WELCH:
			strcpy(options, "      Welch");
			break;
		case FFT_WINDOW_HANN:
			strcpy(options, "       Hann");
			break;
		case FFT_WINDOW_HAMMING:
			strcpy(options, "    Hamming");
			break;
		case FFT_WINDOW_BLACKMAN:
			strcpy(options, "   Blackman");
			break;
		case FFT_WINDOW_NUTTALL:
			strcpy(options, "    Nuttall");
			break;
		}
		break;
	case CONFIG_RESET_SER_EEPROM:
		if(Read_24Cxx(0,8) == 0xFE00)
		{
			strcpy(options, "   n/a");
			clr = Red;
		}
		else
		{
			strcpy(options, "Do it!");
			clr = White;
			opt_pos =  CONFIG_RESET_SER_EEPROM % MENUSIZE;			// Y position of this menu item
			if(var>=1)
			{
				// clear EEPROM
				UiLcdHy28_PrintText(POS_MENU_IND_X+189, POS_MENU_IND_Y+opt_pos*12,"Working",Red,Black,0);
				Write_24Cxx(0,0xFF,16);
				Write_24Cxx(1,0xFF,16);
				ui_si570_get_configuration();		// restore SI570 to factory default
				*(__IO uint32_t*)(SRAM2_BASE) = 0x55;
				NVIC_SystemReset();			// restart mcHF
			}
		}
		break;
	case MENU_1K4_SEL: //
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_1P4KHZ);
		break;
	case MENU_1K6_SEL: //
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_1P6KHZ);
		break;
	case MENU_2K1_SEL: //
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_2P1KHZ);
		break;
	case MENU_2K5_SEL: //
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_2P5KHZ);
		break;
	case MENU_2K9_SEL: // 2.9 kHz filter select
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_2P9KHZ);
		break;
	case MENU_3K2_SEL: //
      UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_3P2KHZ);
		break;
	case MENU_3K4_SEL: //
        UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_3P4KHZ);
		break;
	case MENU_3K8_SEL: //
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_3P8KHZ);
		break;
	case MENU_4K0_SEL:	//
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_4P0KHZ);
		break;
	case MENU_4K2_SEL:	//
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_4P2KHZ);
		break;
	case MENU_4K6_SEL:	//
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_4P6KHZ);
		break;
	case MENU_4K8_SEL:	//
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_4P8KHZ);
		break;
	case MENU_5K0_SEL:	//
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_5P0KHZ);
		break;
	case MENU_5K5_SEL:	//
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_5P5KHZ);
		break;
	case MENU_6K5_SEL:	//
        UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_6P5KHZ);
		break;
	case MENU_7K0_SEL:	//
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_7P0KHZ);
		break;
	case MENU_7K5_SEL:	//
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_7P5KHZ);
		break;
	case MENU_8K0_SEL:	//
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_8P0KHZ);
		break;
	case MENU_8K5_SEL:	//
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_8P5KHZ);
		break;
	case MENU_9K0_SEL:	//
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_9P0KHZ);
		break;
	case MENU_9K5_SEL:	//
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_9P5KHZ);
		break;
	case MENU_10K0_SEL:	//
	    UiMenuHandleFilterConfig(var,mode,options,&clr,AUDIO_10P0KHZ);
		break;

	case CONFIG_DSP_ENABLE:	// Enable DSP NR
		temp_var = ts.dsp_enabled;
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		if(tchange)
		    ts.dsp_enabled = temp_var;
		break;
	case CONFIG_CAT_XLAT:	// CAT xlat reporting
		temp_var = ts.xlat;
		tchange = UiDriverMenuItemChangeEnableOnOff(var, mode, &temp_var,0,options,&clr);
		if(tchange)
		    ts.xlat = temp_var;
		break;
	default:						// Move to this location if we get to the bottom of the table!
		strcpy(options, "ERROR!");
		opt_pos = 5;
		break;
	}
	UiLcdHy28_PrintTextRight(POS_MENU_CURSOR_X - 4, POS_MENU_IND_Y + (opt_pos * 12), options, clr, Black, 0);		// yes, normal position
	if(mode == 1)	{	// Shifted over
		if(opt_oldpos != 999)		// was the position of a previous cursor stored?
			UiLcdHy28_PrintText(POS_MENU_CURSOR_X, POS_MENU_IND_Y + (opt_oldpos * 12), " ", Black, Black, 0);	// yes - erase it
		//
		opt_oldpos = opt_pos;	// save position of new "old" cursor position
		UiLcdHy28_PrintText(POS_MENU_CURSOR_X, POS_MENU_IND_Y + (opt_pos * 12), "<", Green, Black, 0);	// place cursor at active position
	}
	//
	return;
}

//
// This code is under development - EXPECT ERRORS, DAMMIT!
//
//*----------------------------------------------------------------------------
//* Function Name       : UiDriverMemMenu
//* Object              : Drive Display of channel memory data
//* Input Parameters    : none
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
//
void UiDriverMemMenu(void)
{
	static bool update_vars = 1;
	static uchar change_detect = 255;
	static	uchar menu_num = 99;
	static	uchar old_menu_num = 0;
	uchar var;
	char txt[32];

	sprintf(txt, " %d   ", (int)(ts.menu_item));
	UiLcdHy28_PrintText    ((POS_RIT_IND_X + 1), (POS_RIT_IND_Y + 20),txt,White,Grid,0);

	if(change_detect != ts.menu_item)	{	// has menu selection changed?
		update_vars = 1;					// yes - indicate that we should update on-screen info
		//
	}

	if(update_vars)	{						// change detected?
		update_vars = 0;					// yes, reset flag
		change_detect = ts.menu_item;

		// each menu is composed of a fixed number of entries
		// identified by an incrementing index number, so we can
		// derive menu_num from  menu_item number of interest
		menu_num = ts.menu_item / MENUSIZE;
		if(menu_num != old_menu_num)	{
				old_menu_num = menu_num;
				for(var = menu_num * MENUSIZE; var < ((menu_num+1) * MENUSIZE); var++)
							UiDriverUpdateMemLines(var);
		}
	}
}


//*----------------------------------------------------------------------------
//* Function Name       : UiDriverUpdateMemLines
//* Object              : Display channel memory data
//* Input Parameters    : var = memory item location on screen (1-6)
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
//
void UiDriverUpdateMemLines(uchar var)
{
	ulong opt_pos;					// y position of option
	static ulong opt_oldpos = 999;	// y position of option
	// ulong	mem_mode, mem_freq_high, mem_freq_low;		// holders to store the memory that has been read
	// char s[64];						// holder to build frequency information

	opt_pos = (ulong)var;

	//
	char txt[32];
	sprintf(txt, " %d   ", (int)(opt_pos));
	UiLcdHy28_PrintText    ((POS_RIT_IND_X + 1), (POS_RIT_IND_Y + 32),txt,White,Grid,0);

	opt_pos %= 6;		// calculate position of menu item


	UiLcdHy28_PrintText(POS_MENU_IND_X, POS_MENU_IND_Y+(opt_pos * 12) ,"96-Wfall NoSig Adj.",White,Black,0);

	if(opt_oldpos != 999)		// was the position of a previous cursor stored?
		UiLcdHy28_PrintText(POS_MENU_CURSOR_X, POS_MENU_IND_Y + (opt_oldpos * 12), " ", Black, Black, 0);	// yes - erase it
		//
		opt_oldpos = opt_pos;	// save position of new "old" cursor position
		UiLcdHy28_PrintText(POS_MENU_CURSOR_X, POS_MENU_IND_Y + (opt_pos * 12), "<", Green, Black, 0);	// place cursor at active position
//
	return;
}

// -----------------------------------------------
static bool is_last_menu_item = 0;


void UiMenu_RenderChangeItem(int16_t pot_diff) {
  if(pot_diff < 0)    {
        if(ts.menu_item)    {
            ts.menu_item--;
        }
        else    {
            if(!ts.radio_config_menu_enable)
                ts.menu_item = MAX_MENU_ITEM-1; // move to the last menu item (e.g. "wrap around")
            else
                ts.menu_item = (MAX_MENU_ITEM + MAX_RADIO_CONFIG_ITEM)-1;   // move to the last menu item (e.g. "wrap around")
        }
    }
    else    {
        ts.menu_item++;
        if(!ts.radio_config_menu_enable)    {
            if(ts.menu_item >= MAX_MENU_ITEM)   {
                ts.menu_item = 0;   // Note:  ts.menu_item is numbered starting at zero
            }
        }
        else    {
            if(ts.menu_item >= MAX_MENU_ITEM + MAX_RADIO_CONFIG_ITEM)   {
                ts.menu_item = 0;   // Note:  ts.menu_item is numbered starting at zero
            }
        }
    }
    ts.menu_var = 0;            // clear variable that is used to change a menu item
    UiDriverUpdateMenu(1);      // Update that menu item
}

void UiMenu_RenderLastScreen() {
#ifdef NEWMENU
  return;
#endif

  if(ts.menu_item < MAX_MENU_ITEM)    {   // Yes - Is this within the main menu?
      if(ts.menu_item == MAX_MENU_ITEM-1) {   // are we on the LAST menu item of the main menu?
          if(ts.radio_config_menu_enable)     // Yes - is the configuration menu enabled?
              ts.menu_item = MAX_MENU_ITEM;   // yes - go to the FIRST item of the configuration menu
          else                                // configuration menu NOT enabled
              ts.menu_item = 0;               // go to the FIRST menu main menu item
      }
      else                                    // we had not been on the last item of the main menu
          ts.menu_item = MAX_MENU_ITEM-1;     // go to the last item in the main menu
  }
  else    {       // we were NOT in the main menu, but in the configuration menu!
      if(ts.menu_item == (MAX_MENU_ITEM + MAX_RADIO_CONFIG_ITEM-1))       // are we on the last item of the configuration menu?
          ts.menu_item = 0;                   // yes - go to the first item of the main menu
      else    {       // we are NOT on the last item of the configuration menu
          ts.menu_item = (MAX_MENU_ITEM + MAX_RADIO_CONFIG_ITEM) - 1;     // go to the last item in the configuration menu
      }
  }
  UiDriverUpdateMenu(0);  // update menu display
  UiDriverUpdateMenu(1);  // update cursor

}
void UiMenu_RenderFirstScreen() {
#ifdef NEWMENU
  init_done = false;
  UiMenu_DisplayInitMenu(0);
  return;
#endif

  if(ts.menu_item < MAX_MENU_ITEM)    {   // Yes - Is this within the main menu?
        if(ts.menu_item)    // is this NOT the first menu item?
            ts.menu_item = 0;   // yes - set it to the beginning of the first menu
        else    {           // this IS the first menu item
            if(ts.radio_config_menu_enable)     // yes - is the configuration menu enabled?
                ts.menu_item = (MAX_MENU_ITEM + MAX_RADIO_CONFIG_ITEM)-1;   // move to the last config/adjustment menu item
            else                                // configuration menu NOT enabled
                ts.menu_item = MAX_MENU_ITEM - 1;
        }
    }
    else    {       // we are within the CONFIGURATION menu
        if(ts.menu_item > MAX_MENU_ITEM)        // is this NOT at the first entry of the configuration menu?
            ts.menu_item = MAX_MENU_ITEM;   // yes, go to the first entry of the configuration item
        else        // this IS the first entry of the configuration menu
            ts.menu_item = MAX_MENU_ITEM - 1;   // go to the last entry of the main menu
    }
    UiDriverUpdateMenu(0);  // update menu display
    UiDriverUpdateMenu(1);  // update cursor
}
void UiMenu_RenderNextScreen() {
#ifdef NEWMENU
  UiMenu_DisplayMoveSlotsForward(MENUSIZE);
  UiMenu_DisplayInitMenu(0);
  return;
#endif
  //
  if(!ts.radio_config_menu_enable)    {   // Not in config/calibrate menu mode
    if(ts.menu_item == MAX_MENU_ITEM - 1)   {   // already at last item?
      is_last_menu_item = 0;              // make sure flag is clear
      ts.menu_item = 0;                   // go to first item
    }
    else    {   // not at last item - go ahead
      ts.menu_item += 6;
      if(ts.menu_item >= MAX_MENU_ITEM - 1)   {   // were we at last item?
        if(!is_last_menu_item)  {   // have we NOT seen the last menu item flag before?
          ts.menu_item = MAX_MENU_ITEM - 1;   // set to last menu item
          is_last_menu_item = 1;      // set flag indicating that we are at last menu item
        }
        else    {   // last menu item flag was set
          ts.menu_item = 0;               // yes, wrap around
          is_last_menu_item = 0;              // clear flag
        }
      }
    }
  }
  else    {   // in calibrate/adjust menu mode
    if(ts.menu_item == (MAX_MENU_ITEM + MAX_RADIO_CONFIG_ITEM-1))   {   // already at last item?
      is_last_menu_item = 0;              // make sure flag is clear
      ts.menu_item = 0;                   // to to first item
    }
    else    {   // not at last item - go ahead
      if(ts.menu_item < MAX_MENU_ITEM - 1)    {   // are we starting from the adjustment menu?
        if((ts.menu_item + 6) >= MAX_MENU_ITEM) {       // yes - is the next jump past the end of the menu?
          ts.menu_item = MAX_MENU_ITEM-1;     // yes - jump to the last item
        }
        else
          ts.menu_item += 6;  // not at last item - go to next screen
      }
      else    // not on adjustment menu
        ts.menu_item += 6;  // go to next configuration screen
      //
      if(ts.menu_item >= (MAX_MENU_ITEM + MAX_RADIO_CONFIG_ITEM-1))   {   // were we at last item?
        if(!is_last_menu_item)  {   // have we NOT seen the last menu item flag before?
          ts.menu_item = MAX_MENU_ITEM + MAX_RADIO_CONFIG_ITEM-1; // set to last menu item
          is_last_menu_item = 1;      // set flag indicating that we are at last menu item
        }
        else    {   // last menu item flag was set
          ts.menu_item = 0;               // yes, wrap around
          is_last_menu_item = 0;              // clear flag
        }
      }
    }
  }
  //
  ts.menu_var = 0;            // clear variable that is used to change a menu item
  UiDriverUpdateMenu(1);      // Update that menu item
}

void UiMenu_RenderPrevScreen() {
#ifdef NEWMENU
  UiMenu_DisplayMoveSlotsBackwards(MENUSIZE);
  UiMenu_DisplayInitMenu(0);
  return;
#endif
  is_last_menu_item = 0;  // clear last screen detect flag
  if(ts.menu_item < 6)    {   // are we less than one screen away from the beginning?
    if(!ts.radio_config_menu_enable)    // yes - config/adjust menu not enabled?
      ts.menu_item = MAX_MENU_ITEM-1; // yes, go to last item in normal menu
    else
      ts.menu_item = (MAX_MENU_ITEM + MAX_RADIO_CONFIG_ITEM)-1;   // move to the last config/adjustment menu item
  }
  else    {
    if(ts.menu_item < MAX_MENU_ITEM)    // are we in the config menu?
      if(ts.menu_item >= 6)           // yes - are we at least on the second screen?
        ts.menu_item -= 6;              // yes, go to the previous screen
      else                            // we are on the first screen
        ts.menu_item = 0;           // go to the first item
    //
    else if(ts.menu_item > MAX_MENU_ITEM)   {   // are we within the adjustment menu by at least one entry?
      if((ts.menu_item - 6) < MAX_MENU_ITEM)  {   // yes, will the next step be outside the adjustment menu?
        ts.menu_item = MAX_MENU_ITEM;           // yes - go to bottom of adjustment menu
      }
      else                            // we will stay within the adjustment menu
        ts.menu_item -= 6;          // go back to previous page
    }
    else if(ts.menu_item == MAX_MENU_ITEM)  // are we at the bottom of the adjustment menu?
      ts.menu_item --;                // yes - go to the last entry of the adjustment menu
  }
  //              ts.menu_item -= 6;  // not less than 6, so we subtract!
  //
  ts.menu_var = 0;            // clear variable that is used to change a menu item
  UiDriverUpdateMenu(1);      // Update that menu item
}

void UiMenu_RenderChangeItemValue(int16_t pot_diff) {
  if(pot_diff < 0)  {
    ts.menu_var--;      // increment selected item
  }
  else  {
    ts.menu_var++;      // decrement selected item
  }
  //
  UiDriverUpdateMenu(1);        // perform update of selected item
}
