#Create users with this, % is any ip address
CREATE USER username@'%' IDENTIFIED BY 'password';
#These roles will have to be added to your Database with these proper permissions
#Otherwise the program will not work correctly.
CREATE ROLE Editor;
CREATE ROLE Viewer;
CREATE ROLE QC;
CREATE ROLE Creator;
CREATE ROLE Admin;
#Add these permissions for each Role
GRANT ALL PRIVILEGES ON creator.* TO Admin;
GRANT SELECT ON creator.* TO Viewer;
GRANT SELECT, Insert ON creator.* To Creator;
GRANT SELECT, Update ON creator.* To QC;
GRANT SELECT, Update ON creator.* To Editor;
#
#
#Lastly, set default role for user
SET DEFAULT ROLE role FOR username@'%';