January 9:
- 09:45AM: Pitched project idea to Mr. K. Approved. - Both
- 10:15AM: Defined file specifications in FILESPECS.md - Frank
- 10:15AM: Defined language specifications in LANGSPECS.md - Ben
- 2:04PM: Worked a bit on File IO and basic commonly used structs - Frank

January 10:
- 10:17AM: Finished vector and basic table structures, complete basic init and add_row functions for tables - Frank
- 10:18AM: Started on parser and three functions to handle table commands - Ben

January 11:
- 10:14AM: Worked on select_table. Put together file path and open table. - Ben
- 10:16AM: Worked on (finished but untested) code to read / write tables from files - Frank

January 12:
- 9:59AM: Fix bugs / segfaults in file IO code - Frank
- 10:15AM: Worked on create_table. Added user input for CREATE and master_parser - Ben

January 13:
- 10:05AM: Work on export table to CSV functionality - Frank
- 10:15AM: Lots of git actions to merge table functionality into parser - Ben
- 11:34PM: Finished create_table functionality - Ben
- 11:53PM: Finished drop_table functionality - Ben

January 14: 
- 12:36AM: Finished select_table functionality - Ben
- 6:04PM: Finished add_row_cmd functionality - Ben
- 9:08PM: Finished add_col_cmd functionality - Ben

January 15:
- 4:58PM: Implemented global and table "shells" - Ben
- 5:20PM: Added schema and datatype structures to allow for columns to have different datatypes, add a parser that converts a string to formatted data based on the datatype - Frank
- 7:18PM: Finished retrofitting core table functionality to use schema, rewrote tabledebug.c and file IO code to support new formatted tables (and file IO can also read in old formatted tables and write them in the new format) - Frank
- 7:53PM: Worked to merge parser features to support new schema, ADDROW and CREATE now work again - Frank
- 8:39PM: Finally merged parser features with new table style, got ADDCOL working - Frank

January 16:
- 1:23AM: Finished semaphores (mostly) - Ben
- 9:44PM: Implement DELROW - Ben
- 9:59PM: Set up a basic server-client structure, still needs to be intergrated with parser - Frank
- 10:50PM: Implement SETROW - Ben

January 17:
- 12:26AM: Got server-client system to (mostly) work with parser, with a few bugs unfortunately (notably sometimes buffers just seem to get out of sync?) - Frank
- 12:28AM: Tried implementing DELCOL. Limited functionality - Ben
- 12:50AM: Implement PRINT TABLE and PRINT SCHEMA - Ben
- 12:56AM: Fix input-output order desync error on client-side - Frank
