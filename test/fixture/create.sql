create table student (sno char(8), sname char(16), sage int, sgender char (1), 
       primary key ( sno ) );

create index stunameidx on student ( sname );