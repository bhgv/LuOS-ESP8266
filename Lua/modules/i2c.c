/*
 * Whitecat, I2C Lua module
 *
 * Copyright (C) 2015 - 2016
 * IBEROXARXA SERVICIOS INTEGRALES, S.L. & CSS IBÉRICA, S.L.
 * 
 * Author: Jaume Olivé (jolive@iberoxarxa.com / jolive@whitecatboard.org)
 * 
 * All rights reserved.  
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 */

#include "whitecat.h"

//#if LUA_USE_I2C

#include "lua.h"
#include "lauxlib.h"

#include "c_types.h"

#include <drivers/error.h>
#include <drivers/i2c.h>
#include <drivers/i2c-platform.h>

#include "modules.h"


static int li2c_setup( lua_State* L ) {
//    tdriver_error *error;
    int error;

    int total = lua_gettop(L);
    int id = 0; //luaL_checkinteger(L, 1);
    int speed = 100; //luaL_checkinteger(L, 2);
    int sda;
    int scl;

    // Some integrity checks
//    if (!platform_i2c_exists(id)) {
//        return luaL_error(L, "I2C%d does not exist", id);
//    }

//    if ((speed <= 0) || (speed > 1000)) {
//        return luaL_error(L, "Invalid speed");        
//    }
    
//    if (id > NI2CHW) {
        // SDA and SCL is needed
        if (total != 2) {
            return luaL_error(L, "Missing SDA / SCL arguments");                    
        }
        
        sda = luaL_checkinteger(L, 1);
        scl = luaL_checkinteger(L, 2);
        
        if (sda == scl) {
            return luaL_error(L, "SDA / SCL must be different");      
        }
//    }
    
//    printf("sda=%d, scl=%d\n", sda, scl);
    // Setup
//    if ((error = i2c_setup(id, speed, sda, scl))) {
    //if (!( 
    error = platform_i2c_setup(id, (uint8_t)sda, (uint8_t)scl, speed);
    // )) {
//		lua_pushstring(L, "I2C can't setup");
		lua_pushinteger(L, error);
        return 1; //luaL_driver_error(L, "I2C can't setup", error);
//    }

//    return 0;
}

static int li2c_start( lua_State* L ) {
    int id = luaL_checkinteger(L, 1);

    // Some integrity checks
    if (!platform_i2c_exists(id)) {
        return luaL_error(L, "I2C%d does not exist", id);
    }
    
    platform_i2c_send_start(id);

    return 0;
}

static int li2c_stop( lua_State* L ) {
    int id = luaL_checkinteger(L, 1);

    // Some integrity checks
    if (!platform_i2c_exists(id)) {
        return luaL_error(L, "I2C%d does not exist", id);
    }

    platform_i2c_send_stop(id);
    
    return 0;
}

static int li2c_address( lua_State* L ) {
    int id = luaL_checkinteger(L, 1);
    int address = luaL_checkinteger(L, 2);
    int direction = luaL_checkinteger(L, 3);

    // Some integrity checks
    if (!platform_i2c_exists(id)) {
        return luaL_error(L, "I2C%d does not exist", id);
    }

    if (address >= 0b10000000000) {
        return luaL_error(L, "Ivalid address");        
    }
    
    if ((direction != 0) && (direction != 1)) {
        return luaL_error(L, "Ivalid direction");
    }

    lua_pushboolean(L, platform_i2c_send_address(id, address, direction));
    
    return 1;
}

static int li2c_read( lua_State* L ) {
    int id = luaL_checkinteger(L, 1);
    char data;
    
    // Some integrity checks
    if (!platform_i2c_exists(id)) {
        return luaL_error(L, "I2C%d does not exist", id);
    }

    data = platform_i2c_recv_byte(id, 1);
    
    lua_pushinteger(L, data & 0x000000ff);
   
    return 1;
}

static int li2c_write(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    int data = luaL_checkinteger(L, 2);
    
    // Some integrity checks
    if (!platform_i2c_exists(id)) {
        return luaL_error(L, "I2C%d does not exist", id);
    }

    lua_pushboolean(L, platform_i2c_send_byte(id, (char)(data & 0x000000ff)));

    return 1;
}

static const /*luaL_Reg*/ LUA_REG_TYPE li2c[] = {
/*
    {"setup", li2c_setup},
    {"start", li2c_start},
    {"stop", li2c_stop},
    {"address", li2c_address},
    {"read", li2c_read},
    {"write", li2c_write},
    {NULL, NULL}
*/
//  { LSTRKEY( "fb_rle" ),  LFUNCVAL( lu8g_fb_rle ) },

//    {LSTRKEY( "init" ), LFUNCVAL( li2c_setup )},
    {LSTRKEY( "start" ), LFUNCVAL( li2c_start )},
    {LSTRKEY( "stop" ), LFUNCVAL( li2c_stop )},
    {LSTRKEY( "address" ), LFUNCVAL( li2c_address )},
    {LSTRKEY( "read" ), LFUNCVAL( li2c_read )},
    {LSTRKEY( "write" ), LFUNCVAL( li2c_write )},
    {LSTRKEY( "TRANSMITTER" ), LINTVAL( 0 )},
    {LSTRKEY( "RECEIVER" ), LINTVAL( 1 )},
    {LNILKEY, LNILVAL}
};

int luaopen_i2c(lua_State* L) {
//	uint8_t sda = 4;
//	uint8_t scl = 5;
//	
//	platform_i2c_setup(0, sda, scl, 100);
    return 0;
}

MODULE_REGISTER_MAPPED(I2C, i2c, li2c, luaopen_i2c)

//#endif
