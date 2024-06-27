int main(int argc, char* argv[])  
{  
   char* mensaje;  
  
   if(argc > 1)  
   {  
       mensaje = argv[1];  
   }  
   else  
   {  
       mensaje = "CARBONERIA";  
   }  
  
   printf("CARBONERO %s.\n", mensaje);  
  
   return 0;  
} 