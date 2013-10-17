#pragma once

const char* SQL_InsertTest = "INSERT INTO players VALUES(?, ?, ?, ?, ?, ?)" ;
const char* SQL_SelectTest = "SELECT name, pos_x, pos_y, pos_z FROM players WHERE pid=?" ;
const char* SQL_UpdateTest = "UPDATE players SET pos_x=?, pos_y=?, pos_z=?, comment=? WHERE pid=?" ;
const char* SQL_DeleteTest = "DELETE FROM players WHERE pid=?" ;

