/*

create table boards (
 b0 bigint zerofill not null,
 b1 bigint zerofill not null,
 b2 bigint zerofill not null,
 b3 bigint zerofill not null,
 b4 bigint zerofill not null,
 b5 bigint zerofill not null,
 b6 bigint zerofill not null,
 mul bigint zerofill not null
);

alter table boards add primary key(b0,b1,b2,b3,b4,b5,b6)

 */
const mariadb = require('mariadb');
const fs = require('fs');


let boardSize = 64;
let numScanLines = 100000;
let masterBoards = Buffer.alloc(numScanLines * boardSize);

const pool = mariadb.createPool({
	host: 'localhost',
	user:'borgsum',
	password: 'password',
	connectionLimit: 5,
	database:'chess'
});

let totalMul = 0n;

async function asyncFunction() {
	let conn;
	try {

		conn = await pool.getConnection();
		let size = boardSize * numScanLines;
		let file = fs.openSync( "../results/level5.bin", "r" );
		let readPosition =0;

		let numRead = fs.readSync(file, masterBoards, 0, size, readPosition );

		console.log( "read " + numRead + " from readPosition " + readPosition);
		while( numRead > 0) {
			readPosition += numRead;

			for( let i = 0;i<numRead; i+=64) {
				let master = Buffer.alloc(64);
				masterBoards.copy(master, 0, i, i + 64);

				let masterMul = master.readBigUInt64LE(56);
				totalMul += masterMul;
				let keys = makeKeyArray(master);
				let query = "SELECT * from boards where(b0 = " + keys[0] + " AND b1=" + keys[1] + " AND b2=" + keys[2] + " AND b3=" + keys[3] + " AND b4=" + keys[4] + " AND b5=" + keys[5] + " AND b6=" + keys[6] + " )";
				// console.log( query );

				const rows = await conn.query(query);
				if( rows.length === 0 ){
					let insert = "insert into boards value(" + keys[0] + "," + keys[1] + "," + keys[2] + "," + keys[3] + "," + keys[4] + "," + keys[5] + "," + keys[6] + "," + masterMul + ")";
					await conn.query(insert);
				} else {
					let oldMul = rows[0].mul;
					masterMul += BigInt( oldMul );
					let update = "insert into boards value(" + keys[0] + "," + keys[1] + "," + keys[2] + "," + keys[3] + "," + keys[4] + "," + keys[5] + "," + keys[6] + "," + masterMul + ") ON DUPLICATE KEY UPDATE mul = " + masterMul;
					await conn.query(update);
				}

				// console.log(rows);
				//const res = await conn.query("INSERT INTO boards value (?,?,?,?,?,?,?,?)", [2,1,2,4,5,6,7,8274]);
				//console.log(res); // { affectedRows: 1, insertId: 1, warningStatus: 0 }

			}

			numRead = fs.readSync(file, masterBoards, 0, size, readPosition);
			console.log( "read " + numRead + " from readPosition " + readPosition);


		}

		fs.closeSync(file);

		console.log( "done. num boards:", totalMul);

	} catch (err) {
		throw err;
	} finally {
		if (conn){
			conn.end();
			pool.end();
		}
	}
}

function makeKeyArray(buffer) {
	let key = [];
	for (let i = 0; i < 7; i++) {
		key.push( buffer.readBigUInt64LE(i * 8) );
	}
	return key;
}

asyncFunction();


