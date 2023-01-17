# For Personal Tracking - Ben

## Shell management

- [X] Set up global shell
- [X] Fix SELECT path error
- [X] Set up table shell
  - [X] Make sure it exits to global shell
  - [ ] Introduce macro for tprintf to customize table-specific shell
- [ ] Raise error if trying to create table when it already exists
- [X] Don't stop the global/table shell if user inputs wrong things

## Semaphores
- [ ] Vector operations that can be implemented later
  - [ ] find(void *). Could be useful for querying too
  - [ ] erase(index). use find
- [ ] Vector of semaphore keys, each correspoding to a table
  - [X] In create_table, create semaphore, append key and name to file 
  - [X] In drop_table, find key-name pair in file and delete
  - [X] Free unused variables
  - [X] In select_table, find key-name pair in file, read/write??
    Already writing, trying to read (PRINT): Block. Good
    Already reading, trying to write: Block.
    Already writing, trying to write: Block. Need to refresh? Can implement later
    Read, read: No block.
- [ ] Create function to read from key-name pair file
- [X] Semaphore value = INTMAX. Read -= 1, Write -= INTMAX

## Commands
- [ ] Print column data types
- [ ] Random output of col names after commands
- [ ] Update LANGSPECS and add help command
- [X] Fix return values for a lot of functions
- [ ] Fix issue where broken commands like 'exit' doesn't really exist and gets stuck after inputting them twice consecutively