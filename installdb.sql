USE mysql;
DROP FUNCTION IF EXISTS r_ext;
CREATE FUNCTION r_ext RETURNS STRING SONAME 'mysqlr.so';
SELECT r_ext("ls()");
