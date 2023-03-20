
#include <string.h>
#include <fstream.h>
#include <iostream.h>
#include <iomanip.h>

main ( int argc, char *argv[] ){

  char word[200], func[200]; 
  int i, j, n, N, **a ;
  char ls[] = "l(\"\",e(\"\",[a(\"EDGECOLOR\",\"#ffccff\")],r(\""; 


  if ( argc != 3 ){
    cout << "argv[1] = network file (input) " << '\n';
    cout << "argv[2] = daVinci file (output)" << '\n';
    return(0);
  }

  ifstream in ( argv[1] ) ; 
  ofstream out ( argv[2] ) ; 

  // Number of nodes 

  in >> word ; 
  while (!strstr(word,"num_nodes") ) in >> word ;  
  in >> N ;

  // Memory allocation 
    
  a = new int*[N] ; 
  for ( i=0 ; i<N ; i++ ) a[i] = new int [N] ;

  // Read graph 

  while( !strstr(word,"Graph") || (strlen(word)!=5) ) in >> word ;

  for ( i = 0; i<N; i++ ){
    for ( j = 0; j<N; j++ ) in >> a[i][j] ;
  } 

  while( !strstr(word,"Functions") || (strlen(word)!=9) ) in >> word ;  

  out << "  [" << endl ; 
  for ( n=0 ; n<N ; n++ ) {
    in >>  word ; 
    in >>  word ; 
    strcpy ( func , word ) ; 

    out << "     l(\"" << setw(3) << n << "\"," << endl ; 
    out << "       n(\"\"," << endl ; 
    out << "          [ a(\"OBJECT\",\"" << 
      setw(3) << n << "\\"<< 'n' << func << "\")]," << endl ; 
    out << "          [" << endl ; 

    for (i=0 ; i<N ; i++ ){
      if ( a[n][i] == 1 ){
	  out << "           " << ls << setw(3) << i << "\")))," << endl ;
  //final should have (?) no comma 
      }
    }

    out << "          ]" << endl ;
    out << "        )" << endl ; 
    out << "      )," << endl ; //final should have (?) no comma 

  }

  out << "  ]" << endl ; 
  
  in.close() ; 
  out.close() ; 

}
