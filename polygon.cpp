#include "headers.h"

polygon::polygon(std::vector<int> _points) {
   points = _points;
}

void polygon::block(framebuffer f, int startX, int startY, int height, int width, int red, int green, int blue) {
    struct fb_fix_screeninfo finfo = f.getfinfo(); 
    struct fb_var_screeninfo vinfo = f.getvinfo(); 
    char *fbp = f.getfbp();
    int location = 0;
    for (int y = startY; y < startY+height; y++) {
        for (int x = startX; x < startX+width; x++) {
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                       (y+vinfo.yoffset) * finfo.line_length;
            if (vinfo.bits_per_pixel == 32) {
                *(fbp + location) = blue;       // Some bluei
                *(fbp + location + 1) = green;  // A little green
                *(fbp + location + 2) = red;    // A lot of red
                *(fbp + location + 3) = 0;      // No transparency
		//std::cout << ((*((int *) (fbp + location))>>16) & 0xff)<< std::endl;
		//std::cout << ((*((int *) (fbp + location + 1))>>8)& 0xff)<< std::endl;
		//std::cout << (*((int *) (fbp + location + 2))& 0xff)<< std::endl; 
            } else  { //assume 16bpp
                unsigned short int t = red<<11 | green<< 5 | blue;
                *((unsigned short int*)(fbp + location)) = t;
            }
        }
    }
}

void polygon::get_color(int* result,framebuffer f, int x, int y){
	struct fb_fix_screeninfo finfo = f.getfinfo(); 
	struct fb_var_screeninfo vinfo = f.getvinfo(); 
	char *fbp = f.getfbp();
	int location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
	result[0] = *(fbp + location);
	result[1] = *(fbp + location + 1);
	result[2] = *(fbp + location + 2);
	result[0] = ((result[0]>>16) & 0xff);
	result[1] = ((result[1]>>8) & 0xff);
	result[2] = (result[2] & 0xff);
}

void polygon::draw_fill(framebuffer f, int x, int y, int red, int green, int blue) {
  if(x<0 || x>1360 || y<0 || y>500);
  else {  
    int color[3];
    int test_color1[3];
    int test_color2[3];
    int x1 = x;
    int y1 = y;
    do{
      do{
        x1--;
        get_color(color,f,x1,y1);
      }while((color[0] != 0) && (color[1] != 0) && (color[2] != 0));
      x1++;
      do{
	block(f, x1, y1, 1, 1, red, green, blue);
        x1++;
        get_color(color,f,x1,y1);
      }while((color[0] != 0) && (color[1] != 0) && (color[2] != 0));
      y1++;x1=x;
      get_color(color,f,x1,y1);
      if((color[0] == 0) && (color[1] == 0) && (color[2] == 0)){
	//std::cout << "test" << std::endl;
        get_color(test_color1,f,x1+1,y1-1);
        get_color(test_color2,f,x1-1,y1+1);
        if((test_color1[0] == 0) && (test_color1[1] == 0) && (test_color1[2] == 0) && (test_color2[0] == 0) && (test_color2[1] == 0) && (test_color2[2] == 0)){
	   x1--;
        }else{
           get_color(test_color1,f,x1-1,y1-1);
           get_color(test_color2,f,x1+1,y1+1);
	   if((test_color1[0] == 0) && (test_color1[1] == 0) && (test_color1[2] == 0) && (test_color2[0] == 0) && (test_color2[1] == 0) && (test_color2[2] == 0))
	      x1++;
        }
	   
      }
    }while((color[0] != 0) && (color[1] != 0) && (color[2] != 0));
    y1=y-1;
    do{
      do{
        x1--;
        get_color(color,f,x1,y1);
      }while((color[0] != 0) && (color[1] != 0) && (color[2] != 0));
      x1++;
      do{
	block(f, x1, y1, 1, 1, red, green, blue);
        x1++;
        get_color(color,f,x1,y1);
      }while((color[0] != 0) && (color[1] != 0) && (color[2] != 0));
      y1--;x1=x;
      get_color(color,f,x1,y1);
    }while((color[0] != 0) && (color[1] != 0) && (color[2] != 0));
    //if ((color[0] != blue) && (color[1] != green) && (color[2] != red)) {
      //block(f,x,y,1,1,red,green,blue);
      //draw_fill(f, x+1, y, red, green, blue);
      //draw_fill(f, x-1, y, red, green, blue);
      //draw_fill(f, x, y+1, red, green, blue);
      //draw_fill(f, x, y-1, red, green, blue);
    //}
  }
}

void polygon::line(framebuffer f, int x0, int y0, int x1, int y1) {
    int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
    int err = (dx>dy ? dx : -dy)/2, e2;
    for(;;){
  	block(f, x0, y0, 1, 1, 0, 0, 0);
    	if(x0==x1 && y0==y1) break;
    	e2 = err;
    	if(e2 >-dx) { err -= dy; x0 += sx; }
   	if(e2 < dy) { err += dx; y0 += sy; }
    }
}

void *polygon::draw(framebuffer f,bool mode, int scale) {
   int i;
   for(i=0;i<=points.size()-4;i+=2){
      line(f,points[i],points[i+1],points[i+2],points[i+3]);	
   }
   if(mode)   
      line(f,points[i],points[i+1],points[0],points[1]);
}	

void polygon::translation(int x, int y) {
   for(int i=0;i<points.size();i++) 
    points[i] += i%2>0?y:x;
}

void polygon::rotate(int degree) {
	float val = 3.14/180.0;
    std::vector<int> _points;
    for(int i=0;i<_points.size();i++)
        std::cout << _points.at(i) << ' ' << points[i] << '\n';
    for(int i=0;i<points.size();i++) {
	    if(i%2==0) {
			_points.push_back(points.at(i)-poros[0]*cos(degree * val) - points.at(i+1)-poros[1]*sin(degree*val));
		} else {
			_points.push_back(points.at(i-1)-poros[0]*sin(degree * val) + points.at(i)-poros[1]*cos(degree* val));
		}
	}
    for(int i=0;i<_points.size();i++) {
        if(_points[i]<0) _points[i] = 0;
        if(i%2==0 && _points[i]>760) _points[i] = 760;
        if(i%2>0 && _points[i]>1360) _points[i] = 1360;
    }
    this->points = _points;
    //std::cout <<  points.at(points.size()-2)-poros[0]*cos(degree * val) - points.at(points.size()-2)-poros[1]*sin(degree * val) << '\n';
}

void polygon::clip(framebuffer f, int xmin, int ymin, int xmax, int ymax) {}

void bezier(framebuffer f,std::vector<int> v)
{
    int i;
    double t;
    polygon p1(v);
    for (t = 0.0; t < 1.0; t += 0.0005)
    {
        double xt = pow (1-t, 3) * v[0] + 3 * t * pow (1-t, 2) * v[2] + 3 * pow (t, 2) * (1-t) * v[4] + pow (t, 3) * v[6];
        double yt = pow (1-t, 3) * v[1] + 3 * t * pow (1-t, 2) * v[3] + 3 * pow (t, 2) * (1-t) * v[5] + pow (t, 3) * v[7];
        int xt1 = (int) xt;
        int yt1 = (int) yt;
        p1.block(f, xt1, yt1, 1, 1, 255, 55, 250);
    }
}

int main() {
  framebuffer f;
  std::vector<int> points{50,50,100,500,300,400};
  std::vector<int> points1{115,630,83,548,105,505,24,440,47,401,18,306,149,272,148,251,201,240,303,156,446,114,604,111,877,41,915,127,942,209,996,412,982,452,958,445,918,440,915,411,515,521};
  std::vector<int> points2{815,196,772,146,761,152,805,203};
  std::vector<int> points3{757,157,827,236,804,256,731,179};
  std::vector<int> points4{792,268,713,182,689,202,712,226,731,209,761,244,736,265,763,293};
  std::vector<int> points5{678,189,633,228,604,196,592,206,620,238,601,254,571,222,559,233,594,271,582,283,601,305,615,295,647,332,660,321,634,292,654,276,679,302,692,293,669,266,711,228};
  std::vector<int> points6{730,275,684,313,691,326,707,313,713,317,719,312,729,317,751,299,741,289,722,306,714,304,739,283};
  std::vector<int> points7{779,88,791,84,798,109,786,111,782,101,774,104,776,121,751,128,749,122,743,122,743,130,727,133,718,103,733,99,732,97,718,100,715,92,728,87,730,90,742,88,742,86,765,81,771,99,781,97};
  std::vector<int> points8{715,120,719,128,679,149,632,165,632,168,595,179,594,173,561,174,557,150,539,148,535,174,508,171,512,143,517,128,602,125,611,130,619,124,695,105,703,120};
  std::vector<int> points9{692,365,696,364,696,360,700,360,700,363,730,356,732,360,735,361,735,365,733,365,737,380,723,385,724,393,715,398,715,401,711,401,710,398,705,400,700,382,674,388,673,382,693,375};
  std::vector<int> points10{633,395,645,376,637,370,618,396,611,391,607,397,623,409,628,402,631,404,630,407,647,420,667,396,657,388,646,402};
  std::vector<int> points11{703,344,702,354,699,359,671,366,670,363,666,364,673,393,672,408,666,416,659,410,668,396,660,356,664,354,665,357,668,357,667,355,696,346,697,349,701,348};
  std::vector<int> points12{572,438,549,426,560,408,540,397,544,392,533,387,516,417,510,416,518,401,515,398,509,401,505,411,490,429,494,432,492,436,497,437,501,429,520,440,527,433,545,444,550,433,569,443};
  std::vector<int> points13{572,291,567,296,563,291,543,308,545,311,539,316,555,335,551,340,545,339,537,345,537,351,532,354,547,369,552,369,565,362,570,364,579,357,573,349,602,325};
  std::vector<int> points14{426,474,427,476,444,463,451,470,439,481,450,494,453,492,459,497,447,506,438,496,442,492,435,483,421,499,412,490};
  std::vector<int> points15{404,468,404,473,402,473,402,479,351,480,351,468,359,467,359,464,353,463,350,458,351,452,387,449,393,451,391,459,366,462,366,465};
  std::vector<int> points16{304,458,307,456,312,461,307,468,318,481,300,499,302,503,294,510,278,492,266,485,270,481,268,478,276,472,279,474,301,454};
  std::vector<int> points17{102,295,91,305,88,299,83,297,52,309,48,319,62,352,70,355,72,360,62,364,72,389,120,370,116,357,138,347,141,336,131,316,126,318};
  std::vector<int> points18{238,267,244,261,245,263,248,260,236,248,260,226,273,238,282,231,280,229,275,230,265,220,269,217,268,215,286,202,295,215,292,219,293,222,318,200,312,195,318,190,318,186,319,184,316,180,333,163,336,171,341,168,346,173,350,172,355,175,351,177,356,181,351,185,352,189,350,190,353,196,340,208,337,203,334,205,331,202,329,206,320,204,296,225,299,229,314,217,324,227,319,231,320,233,296,253,286,243,289,239,286,235,277,243,289,257,266,280,255,267,245,275};
  std::vector<int> points19{436,148,392,188,396,195,392,198,387,190,378,198,379,201,368,211,367,209,354,220,357,225,360,221,361,225,368,221,372,224,366,229,370,234,386,220,390,224,394,221,395,222,407,215,400,205,395,208,391,204,399,198,404,202,449,164};
  std::vector<int> points20{792,268,713,182,689,202,712,226,731,209,761,244,736,265,763,293};
  std::vector<int> points21{792,268,713,182,689,202,712,226,731,209,761,244,736,265,763,293};
  std::vector<int> points22{792,268,713,182,689,202,712,226,731,209,761,244,736,265,763,293};
  std::vector<int> points23{792,268,713,182,689,202,712,226,731,209,761,244,736,265,763,293};
  std::vector<int> points24{792,268,713,182,689,202,712,226,731,209,761,244,736,265,763,293};
  std::vector<int> points25{792,268,713,182,689,202,712,226,731,209,761,244,736,265,763,293};
  polygon outline(points1);
  polygon parkir_gku(points2);
  polygon gku(points3);
  polygon p3(points4);
  polygon p4(points5);
  polygon p5(points6);
  polygon p6(points7);
  polygon p7(points8);
  polygon p8(points9);
  polygon p9(points10);
  polygon p9a(points11);
  polygon p11(points12);
  polygon p10(points13);
  polygon p12(points14);
  polygon p13(points15);
  polygon p14(points16);
  polygon p15(points17);
  polygon p18(points18);
  polygon p19(points19);
  outline.block(f,0,0,750,1366,255,255,255);
  outline.draw(f,1,2);
  //p.rotate(230);
  //p.draw(f,1,2);
  outline.draw_fill(f,100,300,120,240,0);
  parkir_gku.draw(f,1,2);
  gku.draw(f,1,2);
  p3.draw(f,1,2);
  p4.translation(-5,5); p4.draw(f,1,2); 
  p5.draw(f,1,2);
  p6.draw(f,1,2);
  p7.draw(f,1,2);
  p8.draw(f,1,2);
  p9.draw(f,1,2);
  p9a.draw(f,1,2);
  p11.draw(f,1,2);
  p10.draw(f,1,2);
  p12.draw(f,1,2);
  p13.draw(f,1,2);
  p14.draw(f,1,2);
  p15.draw(f,1,2);
  p18.draw(f,1,2);
  p19.draw(f,1,2);
  return 0;
}
