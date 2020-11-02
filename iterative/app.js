


let buff = new ArrayBuffer(100000);
let boards = new Int8Array(buff);

let currentBoardIndex = 0;
let numBoardsInBuffer = 0;

const NUM_BOARD_BYTES = 5;
const MAX_LEVEL = 10;
let counts = {
	
};

// make initial board
boards[NUM_BOARD_BYTES*currentBoardIndex] = 0; // set move num to 0
boards[NUM_BOARD_BYTES*currentBoardIndex+1] = 0; // not calculated
numBoardsInBuffer++;
let totalBoards = 0;
do {
	if( boards[NUM_BOARD_BYTES*currentBoardIndex] < MAX_LEVEL && boards[NUM_BOARD_BYTES*currentBoardIndex+1] === 0) {
		findAllMovesForCurrentBoard();
	}
	else {
		currentBoardIndex--;
	}
	// console.log( currentBoardIndex, boards[currentBoardIndex*NUM_BOARD_BYTES], boards[currentBoardIndex*NUM_BOARD_BYTES+1] );
	// TODO: Find out what to do next.
	// IS the next board to deep? currentBoardIndex--;
	// IS the current board already calculated? currentBoardIndex--;

}
while( currentBoardIndex !== 0 );

console.log(counts);
console.log(totalBoards);

function count(level,numMoves){
	if( counts[level] === null ||counts[level] === undefined ){
		counts[level] = 0;
	}
	counts[level] += numMoves;
}

function findAllMovesForCurrentBoard() {

	// pretend we find a random number of moves
	let numMovesFound = 4;//Math.floor( Math.random()*3+2);
	totalBoards+=numMovesFound;
	let currentMoveNum = boards[NUM_BOARD_BYTES*currentBoardIndex]; // first byte is move num
	boards[NUM_BOARD_BYTES*currentBoardIndex+1] = 1; // Sett board as calculated
	currentMoveNum++;
	count(currentMoveNum,numMovesFound);
	for( let moves = 0;moves<numMovesFound;moves++){
		currentBoardIndex++;
		boards[NUM_BOARD_BYTES*currentBoardIndex] = currentMoveNum;
		boards[NUM_BOARD_BYTES*currentBoardIndex+1] = 0; // Not calculated
	}
	numBoardsInBuffer += numMovesFound;
	// Todo: count the boards for this level
}
