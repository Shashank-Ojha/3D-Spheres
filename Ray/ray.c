//
// Torbert, 3 February 2014
//
//

#include <stdio.h>
#include <math.h>
#include <unistd.h> // for close
#include <stdlib.h>
#include <string.h>
#include <math.h>

//
#define WIDTH 640
#define HIGHT 480
//

double spheres[3][7] = {{0.500000, 0.500000, 0.166667, 0.166667, 0, 0, 255},
                 {0.833333, 0.500000, 0.500000, 0.166667, 0, 225, 0},
                 {0.333333, 0.666667, 0.666667, 0.333333, 255, 0, 0}};

//Eye
double ex = 0.500000; //Eye
double ey = 0.500000; 
double ez = -1.000000;

double lx= 0.000000; //light
double ly= 1.000000;
double lz= -0.500000;

double dotProduct(double a, double b, double c, double x, double y, double z)
{
   return a*x + b*y + c*z;
}

double quadratic(double a, double b, double c)
{
   double disc = b*b -4.0*a*c;
   if (disc < 0.0) //negative
      return -1.0;
   double tval = (-b - sqrt(disc))/(2.0*a);
   return tval;
}


int main(void)
{
   //Helix Files

   // FILE * fp;
   // fp = fopen("/Users/Shashank/Documents/Parallel Computing/helix.txt", "r");
   // double a, b, c, d;
   // while (fscanf(fp, "%f %f %f %f", &a, &b, &c, &d) != EOF) 
   // {
   //    printf("%lf %lf %lf %lf\n", a, b, c, d);
   // }
   // fclose(fp);
   //

   int rgb[HIGHT][WIDTH][3] ; // red-green-blue for each pixel 
   int x , y ;
   FILE* fout ;
   //
   for( y = 0 ; y < HIGHT ; y++ )
   {
      for( x = 0 ; x < WIDTH ; x++)
      {

         double px = 1.3333*(x+0.5)/WIDTH; //goes from 0 to 1.33333   // There are the pixels on the screen
         double py =  1 - (y+0.5)/HIGHT;  // goes from 0 to 1 
         double pz = 0;

         double rx = px - ex; // This is vector from the eye to the pixels
         double ry = py - ey;
         double rz = pz - ez;

         double mag = sqrt(rx*rx + ry*ry + rz*rz);

         rx /= mag;
         ry /= mag;
         rz /= mag;
         

         // make parametric form of vectors
         // check to see if the vector intersects a sphere and calculate the value of t at the intersection
         // then form quadratic form of T and solve to find t val
         int sphereNum;
         int totalObjects = 4;
         double arrayTVal[4]; 
         for(sphereNum  = 0; sphereNum < 3; sphereNum++)
         {
               double cx = spheres[sphereNum][0];
               double cy = spheres[sphereNum][1];
               double cz = spheres[sphereNum][2];
               double r  = spheres[sphereNum][3];

               double a = 1;
               double b = -2*cx*rx - 2*cy*ry - 2*cz*rz + 2*ex*rx + 2*ey*ry + 2*ez*rz;
               double c = cx*cx - 2*cx*ex + ex*ex + cy*cy - 2*cy*ey + ey*ey + cz*cz - 2*cz*ez + ez*ez - r*r;

               double tVal = quadratic(a, b, c);

               arrayTVal[sphereNum] = tVal;
         }
         double floorT = (0.33333333 - ey)/ry;
         arrayTVal[3] = floorT;
         //find which object has min t;
         int minObject = -1;
         double minValue1 = 1000000000;
         int objectNum;
         for(objectNum  = 0; objectNum < totalObjects; objectNum++)
         {
            if(arrayTVal[objectNum] < minValue1 && arrayTVal[objectNum] > 0)
            {
               minValue1 = arrayTVal[objectNum];
               minObject = objectNum;
            }
         }

         px = ex + minValue1 * rx ;
         py = ey + minValue1 * ry ;
         pz = ez + minValue1 * rz ;

         double nx,ny,nz;

         if (minObject != 3) //sphere
         {
            rgb[y][x][0] = spheres[minObject][4]; // red
            rgb[y][x][1] = spheres[minObject][5]; // green
            rgb[y][x][2] = spheres[minObject][6]; // blue
            nx = (px-spheres[minObject][0])/spheres[minObject][3]; // px-
            ny = (py-spheres[minObject][1])/spheres[minObject][3];
            nz = (pz-spheres[minObject][2])/spheres[minObject][3];

         }
         else //floor
         {
            rgb[y][x][0] = 255; // red
            rgb[y][x][1] = 215; // green
            rgb[y][x][2] = 0; // blue
            //
            nx=0; ny=1; nz=0;
         }

         px += (0.001*nx);
         py += (0.001*ny);
         pz += (0.001*nz);
   


         // // Repeat Process with the light

            double rx2 = lx - px; // This is vector from the pixels to the light
            double ry2 = ly - py;
            double rz2 = lz - pz;

            double mag2 = sqrt(rx2*rx2 + ry2*ry2 + rz2*rz2);

            rx2 /= mag2;
            ry2 /= mag2;
            rz2 /= mag2;
            for(sphereNum  = 0; sphereNum < 3; sphereNum++)
            {

                  double cx = spheres[sphereNum][0];
                  double cy = spheres[sphereNum][1];
                  double cz = spheres[sphereNum][2];
                  double r  = spheres[sphereNum][3];

                  double a = 1;
                  double b = -2*cx*rx2 - 2*cy*ry2 - 2*cz*rz2 + 2*px*rx2 + 2*py*ry2 + 2*pz*rz2;
                  double c = cx*cx - 2*cx*px + px*px + cy*cy - 2*cy*py + py*py + cz*cz - 2*cz*pz + pz*pz - r*r;

                  double tVal = quadratic(a, b, c);

                  arrayTVal[sphereNum] = tVal;

                  if(tVal > 0) // this means the point on the screen is obstructed from the light by a sphere
                  {
                     double factor = 0.5;
                     rgb[y][x][0] = rgb[y][x][0]*factor; // red
                     rgb[y][x][1] = rgb[y][x][1]*factor; // green
                     rgb[y][x][2] = rgb[y][x][2]*factor; // blue
                  }
                  else
                  {
                     double dot = dotProduct(nx, ny, nz, -1*rx2*tVal, -1*ry2*tVal, -1*rz2*tVal);
                     if(dot < 0)
                        dot = 0;
                     double factor = 0.5 + 0.5*dot;
                     rgb[y][x][0] = rgb[y][x][0]*factor; // red
                     rgb[y][x][1] = rgb[y][x][1]*factor; // green
                     rgb[y][x][2] = rgb[y][x][2]*factor; // blue
                  }
           }

      }
   }

   //
   fout = fopen( "allgreen.ppm" , "w" ) ;
   //
   fprintf( fout , "P3\n" ) ;
   fprintf( fout , "%d %d\n" , WIDTH , HIGHT ) ;
   fprintf( fout , "255\n" ) ;
   //
   for( y = 0 ; y < HIGHT ; y++ )
   {
      for( x = 0 ; x < WIDTH ; x++)
      {
         fprintf( fout , "%d %d %d\n" ,
          rgb[y][x][0] , rgb[y][x][1] , rgb[y][x][2] ) ;
      }
   }
   // close( fout ) ;
   //
   return 0 ;
}


// end of file
//
// gcc -lm ray.c
// ./a.out
// display allgreen.ppm