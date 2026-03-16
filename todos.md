# Features (WIP)

- [x] adding todos
- [x] deleting todos
- [x] editing todos
- [x] managing todos
- [x] searching todos
- [x] filtering by category
- [x] saving todos to file and loading them on start
- [ ] chained todos
- [ ] maybe a way to display todos that havent been accessed in a while
- [ ] proper display of due_date (just a number right now thats also not really referenced anymore, its meant to be the number of days from the creation until the deadline, but then i have to calculate that timestamp still)
- [ ] make due_date actually usable (and think about how to make it usable)
- [ ] build on windows and arm64 (linux)
- [x] make description search not search for an exact match but instead do a contains search

- [ ] make a proper header file with string/bool and so on so i dont have to keep including it in different files everywhere, it makes everything so messy
## just small notes for learning c
- use `const` more in function parameters that are not modified or freed inside the function (very idiomatic)
