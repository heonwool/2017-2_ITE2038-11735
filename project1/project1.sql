USE Pokemon;
SELECT name FROM Trainer WHERE hometown = 'Blue City';
SELECT name FROM Trainer WHERE hometown = 'Brown City' or hometown = 'Rainbow City';
SELECT name, hometown FROM Trainer WHERE (name LIKE 'A%' OR name LIKE 'E%' OR name LIKE 'I%' OR name LIKE 'O%' OR name LIKE 'U%');
SELECT name FROM Pokemon WHERE type = 'Water';
SELECT DISTINCT type FROM Pokemon;

SELECT name FROM Pokemon ORDER BY name ASC;
SELECT name FROM Pokemon WHERE name LIKE '%s' OR name LIKE '%S';
SELECT name FROM Pokemon WHERE name LIKE '%e%s';
SELECT name FROM Pokemon WHERE (name LIKE 'A%' OR name LIKE 'E%' OR name LIKE 'I%' OR name LIKE 'O%' OR name LIKE 'U%');
SELECT type, COUNT(*) FROM Pokemon GROUP BY type; 

SELECT nickname FROM CatchedPokemon ORDER BY level DESC LIMIT 3;
SELECT AVG(level) FROM CatchedPokemon;
SELECT (MAX(level) - MIN(level)) AS 'gap' FROM CatchedPokemon;
SELECT LEFT(name, 1) AS 'First Letter', COUNT(*) FROM Pokemon WHERE LEFT(name, 1) BETWEEN 'B' AND 'E' GROUP BY LEFT(name, 1);
SELECT COUNT(*) FROM Pokemon WHERE NOT type IN ('Fire', 'Grass', 'Water', 'Electric');

SELECT Trainer.name AS 'trainer', Pokemon.name AS 'pokemon', C.nickname FROM CatchedPokemon C JOIN Trainer ON C.owner_id = Trainer.id JOIN Pokemon ON C.pid = Pokemon.id;
SELECT Trainer.name AS 'trainer' FROM CatchedPokemon C JOIN Trainer ON C.owner_id = Trainer.id JOIN Pokemon ON C.pid = Pokemon.id WHERE Pokemon.type = 'Psychic';
SELECT T.name AS 'Trainer', AVG(C.level) FROM CatchedPokemon C JOIN Trainer T ON C.owner_id = T.id GROUP BY C.owner_id ORDER BY AVG(C.level) DESC LIMIT 3;
SELECT T.name AS 'Trainer', COUNT(*) FROM CatchedPokemon C JOIN Trainer T ON C.owner_id = T.id GROUP BY C.owner_id ORDER BY COUNT(*) DESC, T.name DESC;
SELECT P.name, C.level FROM Gym G JOIN CatchedPokemon C ON C.owner_id = G.leader_id JOIN Pokemon P ON C.pid = P.id WHERE city = 'Sangnok City' ORDER BY C.level ASC;


SELECT P.name, SUM(IF(C.owner_id, 1, 0)) FROM Pokemon P LEFT JOIN CatchedPokemon C ON P.id = C.pid GROUP BY P.id ORDER BY SUM(IF(C.owner_id, 1, 0)) DESC;
SELECT P.name FROM Pokemon P WHERE P.id = (SELECT E.after_id FROM Evolution E WHERE E.before_id = (SELECT E.after_id FROM Evolution E JOIN Pokemon P ON P.id = E.before_id WHERE P.name = 'Charmander'));
SELECT DISTINCT P.name FROM Pokemon P JOIN CatchedPokemon C ON C.pid = P.id WHERE P.id <= 30;
SELECT T.name, P.type FROM Trainer T JOIN CatchedPokemon C ON T.id = C.owner_id JOIN Pokemon P on P.id = C.pid GROUP BY T.name HAVING COUNT(DISTINCT P.type) = 1;
SELECT T.name, P.type, COUNT(P.type) AS '#OfType' FROM Trainer T JOIN CatchedPokemon C ON T.id = C.owner_id JOIN Pokemon P ON C.pid = P.id GROUP BY T.name, P.type ORDER BY T.name, P.type, COUNT(P.type);

SELECT T.name AS 'Trainer', P.name AS 'Pokemon', COUNT(P.id) AS '#OfCaptured' FROM Trainer T JOIN CatchedPokemon C ON T.id = C.owner_id JOIN Pokemon P on P.id = C.pid GROUP BY T.name HAVING COUNT(DISTINCT P.id) = 1;
SELECT T.name, G.city FROM Gym G JOIN Trainer T ON T.id = G.leader_id JOIN CatchedPokemon C ON C.owner_id = G.leader_id JOIN Pokemon P ON P.id = C.pid GROUP BY G.leader_id HAVING COUNT(DISTINCT P.type) != 1;
SELECT T.name, SUM(COND.level) AS 'Sum of level (50 or above)' FROM Trainer T, (SELECT G.leader_id, C.level FROM Gym G LEFT JOIN CatchedPokemon C ON G.leader_id = C.owner_id AND C.level >= 50) COND WHERE T.id = COND.leader_id GROUP BY T.name;
SELECT P.name FROM CatchedPokemon C JOIN Pokemon P ON P.id = C.pid JOIN Trainer T ON T.id = C.owner_id AND T.hometown IN ('Sangnok City') WHERE C.pid IN (SELECT C.pid from CatchedPokemon C JOIN Trainer T ON T.id = C.owner_id AND T.hometown IN ('Blue City'));
SELECT P.name FROM Evolution E1 JOIN Pokemon P ON E1.after_id = P.id WHERE NOT EXISTS (SELECT E2.before_id FROM Evolution E2 WHERE E1.after_id = E2.before_id);
