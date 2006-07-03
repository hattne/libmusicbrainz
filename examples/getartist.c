/* --------------------------------------------------------------------------

   MusicBrainz -- The Internet music metadatabase

   Copyright (C) 2000 Robert Kaye
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

     $Id$

----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "musicbrainz/mb_c.h"

int main(int argc, char *argv[])
{
    musicbrainz_t o;
    char          error[256], data[256], temp[256], *args[2];
    int           ret;

    if (argc < 2)
    {
        printf("Usage: getartist <artistid>\n");
        exit(0);
    }

    // Create the musicbrainz object, which will be needed for subsequent calls
    o = mb_New();

#ifdef WIN32
    mb_WSAInit(o);
#endif

    // Tell the client library to return data in ISO8859-1 and not UTF-8
    mb_UseUTF8(o, 0);

    // Tell the client library to print query and response info to stdout 
    mb_SetDebug(o, 1);

    // Set the proper server to use. Defaults to mm.musicbrainz.org:80
    if (getenv("MB_SERVER"))
        mb_SetServer(o, getenv("MB_SERVER"), 80);

    // Check to see if the debug env var has been set 
    if (getenv("MB_DEBUG"))
        mb_SetDebug(o, atoi(getenv("MB_DEBUG")));

    // Tell the server to only return 2 levels of data, unless the MB_DEPTH env var is set
    if (getenv("MB_DEPTH"))
        mb_SetDepth(o, atoi(getenv("MB_DEPTH")));
    else
        mb_SetDepth(o, 2);

    // Set up the args for the find artist query
    args[0] = argv[1];
    args[1] = NULL;

    // Execute the MB_GetArtistById query
    ret = mb_QueryWithArgs(o, MBQ_GetArtistById, args);
    if (!ret)
    {
        mb_GetQueryError(o, error, 256);
        printf("Query failed: %s\n", error);
        mb_Delete(o);
        return 0;
    }

    // Select the first artist
    mb_Select1(o, MBS_SelectArtist, 1);

    // Pull back the album id to see if we got the album
    if (!mb_GetResultData(o, MBE_ArtistGetArtistId, data, 256))
    {
        printf("Artist not found.\n");
        mb_Delete(o);
        return 0;
    }
    mb_GetIDFromURL(o, data, temp, 256);
    printf(" ArtistId: %s\n", temp);

    // Extract the artist name
    if (mb_GetResultData(o, MBE_ArtistGetArtistName, data, 256))
       printf("     Name: %s\n", data);   

    // Extract the sort name
    if (mb_GetResultData(o, MBE_ArtistGetArtistSortName, data, 256))
       printf(" SortName: %s\n", data);    

#ifdef WIN32
    mb_WSAStop(o);
#endif

    // and clean up the musicbrainz object
    mb_Delete(o);

    return 0;
}