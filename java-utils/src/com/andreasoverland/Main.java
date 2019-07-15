package com.andreasoverland;

import java.math.BigInteger;

public class Main {

    public static void main(String[] args) {

        Long sum = 0L;

        Long l = Long.parseLong( "AAAAAAAA",11) + 1;

        sum += l;

        System.out.println( "\t"+ l + " = number of ways Q_e can be combined on one line, without kings" );

        l = Long.parseLong( "AAAAAAA",11) + 1;
        sum += 16*l;

        System.out.println( "\t"+ sum + " = number of ways Q_e can be combined on one line, including zero or one king" );

        l = Long.parseLong( "AAAAAA",11) + 1;
        sum += 42*l;

        System.out.println( "\t"+ sum + " = number of ways Q_e can be combined on one line, including zero, one or two kings" );

        // Howto make all lines
        // Count from 0 to 214358880
        // For each row, make 16 K|k positions and 42 K&k positions.

        /*

        K.kkkkkk
        .K.kkkkk
        k.K.kkkk
        kk.K.kkk
        kkk.K.kk
        kkkk.K.k
        kkkkk.K.
        kkkkkk.K

         */

        int Qe = Integer.parseInt( "AAAAAAAA",11);

        int numRows = 0;

        int MAX = Qe;

        for( int a=0;a<=MAX;a++){
            String row = Integer.toString( a, 11 );
            while( row.length() < 8 ){
                row = "0" + row;
            }


            if( MAX < 2000 ){
                System.out.println( row );
            }

            numRows ++;

            /*row = row.replaceAll(""," ");
            row = row.replaceAll("0",".");
            row = row.replaceAll("1","P");
            row = row.replaceAll("2","p");
            row = row.replaceAll("3","R");
            row = row.replaceAll("4","r");
            row = row.replaceAll("5","N");
            row = row.replaceAll("6","n");
            row = row.replaceAll("7","B");
            row = row.replaceAll("8","b");
            row = row.replaceAll("9","Q");
            row = row.replaceAll("A","q");*/

            for( int k=0;k<8;k++){

                String replaced = row.substring(k,k+1);

                if( replaced.equals("0") ) {

                    if( MAX < 2000 ) {
                        String newRow = row.substring(0, k) + "B" + row.substring(k + 1);
                        System.out.println(newRow);
                        newRow = row.substring(0, k) + "C" + row.substring(k + 1);
                        System.out.println(newRow);
                    }
                    numRows+=2;

                    for( int k2 = 0;k2<8;k2++){
                        if( k == k2 || k -1 == k2 || k+1 == k2 ){
                            continue;
                        }

                        String replaced2 = row.substring(k2,k2+1);
                        if( replaced2.equals("0") ) {

                            if( MAX < 2000 ) {
                                String newRow = row.substring(0, k) + "B" + row.substring(k + 1);
                                newRow = newRow.substring(0,k2) + "C" + newRow.substring(k2+1);
                                System.out.println(newRow);

                                newRow = row.substring(0, k) + "C" + row.substring(k + 1);
                                newRow = newRow.substring(0,k2) + "B" + newRow.substring(k2+1);
                                System.out.println(newRow);
                            }
                            numRows+=2;
                        }
                    }
                }
            }
        }

        System.out.println( "num rows:" + numRows );
    }


}
