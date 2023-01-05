# Competitive C Club

# Members
Ben Pan

Frank Wong
        
# A statement of the problem you are solving and/or a high level description of the project.

We aim to implement a basic database management system (similar to Sqlite, Name: TBD) and a corresponding language (similar to SQL, Name: TBD). 
    
# A description as to how the project will be used (describe the user interface).

The database will be stored locally. Multiple users will be able to access the database ("SELECT"), but only one will be able to make modifications (database will lock for a very short period of time) using our language.
  
# A description of your technical design. This should include:
   
### How you will be using the topics covered in class in the project.

| Technique                       | Description                                     |
|---------------------------------|-------------------------------------------------|
| Memory Allocation               | Store query return values                       |
| Working with files              | Access and modify database files and tables     |
| Processes (fork)                | Testing concurrency for multiple users          |
| Sockets                         | Handle multiple connections from users          |
| Finding information about files | Get information about database files and tables |
     
### How you are breaking down the project and who is responsible for which parts.
     
### What algorithms and data structures you will be using, and how.

We will start with basic data structures like arrays and linked lists for implementing the database tables/files.

Parsing might require the use of a tree. 

If time allows, we will implement a [B-Tree](https://en.wikipedia.org/wiki/B-tree) which allows search, deletions, and insertions to be done in O(log n) time. For now, we will use linear search and arrays/linked lists instead. 

Another feature we want to implement is the relational model.
    
# A timeline with expected completion dates of parts of the project.
