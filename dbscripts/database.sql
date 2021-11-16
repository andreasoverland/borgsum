drop table if exists king_maps;

create table king_maps (
	id int unsigned auto_increment,
	map bigint unsigned not null unique,
	primary key (id)
);

drop table if exists boards; 
create table boards (
	id bigint unsigned auto_increment,
	boardmaps tinyblob unique, # 255 bytes with long maps : pawns, rooks, knights, bishops, queens, kings, flags (castling squares and ep square)
	primary key (id)
);

create table counts (
	board_id bigint unsigned,
	ply int unsigned,
	counter bigint unsigned,
	primary key( board_id, ply )
);
