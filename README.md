# Competitive C Club

# Members
- Ben Pan
- Frank Wong

Period: **3**
        
# A statement of the problem you are solving and/or a high level description of the project.

We aim to implement a basic database management system (similar to Sqlite, Name: TBD) and a corresponding language (similar to SQL, Name: TBD). 
    
# A description as to how the project will be used (describe the user interface).

The database will be stored locally. Multiple users will be able to access the database ("SELECT"), but only one will be able to make modifications (database will lock for a very short period of time) using our language.
  
# A description of your technical design. This should include:
   
### How you will be using the topics covered in class in the project.

| Technique                       | Description                                                      |
|---------------------------------|------------------------------------------------------------------|
| Memory Allocation               | Store query return values, vectors                               |
| Working with files              | Access and modify database files and tables                      |
| Processes (fork)                | Testing concurrency for multiple users                           |
| Sockets                         | Handle multiple connections from users                           |
| Semaphores                      | Prevent multiple users from modifying the same data concurrently |
| Finding information about files | Get information about database files and tables                  |
     
### How you are breaking down the project and who is responsible for which parts. 

- Phase 1a: Ben & Frank. Straightforward implementation, so will split work evenly

- Phase 1b: Ben

- Phase 2: Frank

- Phase 3 and 4: Ben & Frank since these are advanced features that require further research together

### What algorithms and data structures you will be using, and how.

We will start with basic data structures like arrays and linked lists for implementing the database tables/files.

Parsing might require the use of a tree. 

If time allows, we will implement a [B-Tree](https://en.wikipedia.org/wiki/B-tree) which allows search, deletions, and insertions to be done in O(log n) time. For now, we will use linear search and arrays/linked lists instead. 

Another feature we want to implement is the relational model.
    
# A timeline with expected completion dates of parts of the project.

## Phase 1a: Minimum Viable Product (~3 days)
- Local single process database editor, no sockets or semaphores yet
- Allow basic fixed-form queries (eg. `INSERT INTO table_name VALUES (a, b, c)`, no language parsing, multiple arguments etc.) yet
- Dynamic 1D and 2D array structs (likely void \* vectors)
- Read / write from file with custom format for persistence
- Table likely restricted in terms of data types (integers only)

## Phase 1b: Additional non-networking features (~2-3 days)
- Semaphore for file access management (no writing by multiple processes at once)
- Better database schemas to allow for multiple data types (eg. strings, floating-point numbers)
- Export to CSV file functionality to allow us to see the database with a standard format

## Phase 2: Multiple / socket-based connections (~2 days)
- Forking server & socket-based system to allow for multiple clients to connect to database server
- Different client code that allows you to connect to a server

## Phase 3: Advanced Queries, Features (~3-4 days)
- Better parsing code to allow for more complicated queries (eg. AND in conditions, wildcard support beyond `SELECT * FROM table_name`)
- Client authentication, eg. require credentials for client to modify table
- Upgrade schema to support primary key / unique specifiers, basic relational model support

## Phase 4: Optimisation (~2-3 days)
- Use data structures (eg. aforementioned B-Tree) to speed operations up
- Improve parsing algorithm if needed to support more edge cases / be quicker
