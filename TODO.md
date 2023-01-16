# For Personal Tracking - Ben

## Shell management

- [X] Set up global shell
- [X] Fix SELECT path error
- [X] Set up table shell
  - [X] Make sure it exits to global shell
  - [ ] Introduce macro for tprintf to customize table-specific shell
- [ ] Raise error if trying to create table when it already exists
- [ ] Don't stop the global/table shell if user inputs wrong things

## Semaphores
- [ ] Vector operations that can be implemented later
  - [ ] find(void *). Could be useful for querying too
  - [ ] erase(index). use find
- [ ] Vector of semaphore keys, each correspoding to a table
  - [X] In create_table, create semaphore, append key and name to file 
  - [X] In drop_table, find key-name pair in file and delete
  - [X] Free unused variables
  - [ ] In select_table, find key-name pair in file, read/write??
    Already writing, trying to read (PRINT): Block. Good
    Already reading, trying to write: Block.
    Already writing, trying to write: Block. Need to refresh? Can implement later
    Read, read: No block.
- [ ] Reorganize function to read from key-name pair file
- [ ] Semaphore value = INTMAX. Read -= 1, Write -= INTMAX