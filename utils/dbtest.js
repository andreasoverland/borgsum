/*

create table nboards (
 b0 bigint zerofill not null,
 b1 bigint zerofill not null,
 b2 bigint zerofill not null,
 mul bigint zerofill not null
);

alter table nboards add primary key(b0,b1,b2);

 */
const mariadb = require('mariadb');
const fs = require('fs');


let boardSize = 33;
let numScanLines = 100000;
let masterBoards = Buffer.alloc(numScanLines * boardSize);

const pool = mariadb.createPool({
	host: 'localhost',
	user:'borgsum',
	password: 'password',
	connectionLimit: 1,
	database:'chess'
});

let totalMul = 0n;

async function asyncFunction() {
	let conn;
	try {

		conn = await pool.getConnection();
		/*let masterMul = 5;
		let update = "insert into nboards (b0,b1,b2,mul) value(1,1,1,"+masterMul+") ON DUPLICATE KEY UPDATE mul = mul +" + masterMul;
		await conn.query(update);*/

		
		let size = boardSize * numScanLines;
		let file = fs.openSync( "../results/level6.nbin", "r" );
		let readPosition =0;

		let numRead = fs.readSync(file, masterBoards, 0, size, readPosition );

		console.log( "read " + numRead + " from readPosition " + readPosition);
		while( numRead > 0) {
			await conn.beginTransaction();
			readPosition += numRead;

			for( let i = 0;i<numRead; i+=33) {
				let master = Buffer.alloc(33);
				masterBoards.copy(master, 0, i, i + 33);

				let masterMul = master.readBigUInt64LE(25);
				totalMul += masterMul;
				
				let keys = makeKeyArray(master);

				let update = "insert into nboards value(" + keys[0] + "," + keys[1] + "," + keys[2] + "," + masterMul + ") ON DUPLICATE KEY UPDATE mul = mul +" + masterMul;
				await conn.query(update);

			}

			await conn.commit();

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
	for (let i = 0; i < 3; i++) {
		key.push( buffer.readBigUInt64LE(i * 8) );
	}
	return key;
}

function errorHandler( err, res ){
	console.log( err, res );
}

asyncFunction();
