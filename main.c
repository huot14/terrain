//
//  main.c
//  A2_terrain
//
//  Created by Cole Huot on 2015-10-31.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

int Mode = 1; // 1,2,or 3 for fill, wireframe, or fill + wireframe mode
int lightSwitch = 1;  // lights initially on
int shadeSwitch = 1;  // shading initially smooth

float eye[] = {50,10,50};   // camera position
float look[] = {0,0,50};   // where the camera is looking

void yCameraLeft(){     //camera turn CCW
    if(look[0]>=50){
        if(look[2]>50){
            look[0] += 1;
            look[2] -= 1;
        }
        else{
            look[0] -= 1;
            look[2] -= 1;
        }
    }
    else{
        if(look[2]>50){
            look[0] += 1;
            look[2] += 1;
        }
        else{
            look[0] -= 1;
            look[2] += 1;
        }
    }
}

void yCameraRight(){    //camera turn CW
    if(look[0]>=50){
        if(look[2]<50){
            look[0] += 1;
            look[2] += 1;
        }

        else{
            look[0] -= 1;
            look[2] += 1;

        }
    }
    else{
        if(look[2]<50){
            look[0] += 1;
            look[2] -= 1;
        }
        
        else{
            look[0] -= 1;
            look[2] -= 1;
            
        }
    }
}
void xzCameraUp(){  // camera look up
    if(look[1] < 18){
        look[1] += 1;
    }
}
void xzCameraDown(){    // camera look down
    if(look[1] > -18){
        look[1] -= 1;
    }
}

//light0
float pos0[] = {0,10,0,1};
float amb0[] = {1, 1, 1, 1};
float diff0[] = {1, 1, 0, 1};
float spec0[] = {1, 0, 0, 1};

//light1
float pos1[] = {50,10,50,1};
float amb1[] = {1, 1, 1, 1};
float diff1[] = {0, 1, 0, 1};
float spec1[] = {1, 0, 0, 1};

//material
float m_amb[] = {0.0, 0.5, 0.0, 1.0};
float m_diff[] = {0.78, 0.57, 0.11, 1.0};
float m_spec[] = {0.0, 1, 0.0, 1.0};
float shiny = 27;

int size = 100;  // Initial size of terrain mesh
float heights[300][300];    //Support height values for up to a 300x300 mesh
float maxHeight;
float minHeight = 0;

void mapReset(){    // clears the height values
    int x = 0;
    int z = 0;
    while (x<size){
        while(z<size){
            heights[x][z] = 0;
            z++;
        }
        z = 0;
        x++;
    }
}

void heightmap(){   // Loads the heights array with values
    //http://www.lighthouse3d.com/opengl/terrain/index.php?circles Algorithm Used
    int i = 0;
    maxHeight = 0;
    minHeight = 0;
    while(i < size*size/10){
        //choose random point x0,z0
        int x0 = rand() % size;
        int z0 = rand() % size;
        int x = 0;
        int z = 0;
        int circle = rand() % 10 + 8;   //terrain circle size
        float disp = (rand() % 50)/30; //random displacement
    
        while (x<size){
            while(z<size){
            
                float d = sqrt((pow((x0-x),2))+(pow((z0-z),2))); //distance from centre
                float pd = d * 2.0 / circle;
            
                if (fabs(pd) <= 1.0){
                    heights[x][z] += disp/2.0 + cos(pd*3.14)*disp/2.0;
                }
                if(heights[x][z] > maxHeight){
                    maxHeight = heights[x][z];
                }
                if(heights[x][z] < minHeight){
                    minHeight = heights[x][z];
                }
                z++;
            }
            z = 0;
            x++;
        }
        i++;
    }
}

void heightUp(int mx, int my){  // Displace terrain upwards where the user clicks on the map
    int x0 = mx/(300/size);
    int z0 = (300-my)/(300/size);
    int x = 0;
    int z = 0;
    int circle = 18;   //terrain circle size
    float disp = 1; //displacement
    minHeight = 0;
    maxHeight = 0;
    
    while (x<size){
        while(z<size){
            
            float d = sqrt((pow((x0-x),2))+(pow((z0-z),2))); //distance from centre
            float pd = d * 2.0 / circle;
            
            if (fabs(pd) <= 1.0){
                heights[x][z] += disp/2.0 + cos(pd*3.14)*disp/2.0;

            }
            if(heights[x][z] > maxHeight){
                maxHeight = heights[x][z];
            }
            if(heights[x][z] < minHeight){
                minHeight = heights[x][z];
            }
            z++;
        }
        z = 0;
        x++;
    }

}

void heightDown(int mx, int my){    // Displace terrain downwards where the user clicks on the map
    int x0 = mx/(300/size);
    int z0 = (300-my)/(300/size);
    int x = 0;
    int z = 0;
    int circle = 18;   //terrain circle size
    float disp = -1; //random displacement (negative)
    minHeight = 0;
    maxHeight = 0;
    
    while (x<size){
        while(z<size){
            
            float d = sqrt((pow((x0-x),2))+(pow((z0-z),2))); //distance from centre
            float pd = d * 2.0 / circle;
            
            if (fabs(pd) <= 1.0){
                heights[x][z] += disp/2.0 + cos(pd*3.14)*disp/2.0;
            }
            if(heights[x][z] > maxHeight){
                maxHeight = heights[x][z];
            }
            if(heights[x][z] < minHeight){
                minHeight = heights[x][z];
            }
            z++;
        }
        z = 0;
        x++;
    }
}

// For calculating normals for lighting
float norm_i(float u2,float u3,float v2,float v3){
    return (u2*v3 - u3*v2);
}

float norm_j(float u1,float u3,float v1,float v3){
    return (u3*v1 - u1*v3);
}

float norm_k(float u1,float u2,float v1,float v2){
    return (u1*v2 - u2*v1);
}

float magnitude(float x,float y,float z){
    return fabsf(sqrtf(pow(x,2)+pow(y,2)+pow(z,2)));
}

void drawTerrain()  // Draws the terrain mesh
{

    int x = 0;
    int z = 0;
    
    while(x<size){
        while(z<size){
            float h1 = heights[x][z];
            float h2 = heights[x][z+1];
            float h3 = heights[x+1][z+1];
            float h4 = heights[x+1][z];
            
            // Calculating normals for lighting
            // u1,u2,u3 // 1,h4-h1,0 // // v1,v2,v3 // 0,h2-h1,1
            float n1_i = norm_i(h4-h1,0,h2-h1,1);
            float n1_j = norm_j(1,0,0,1);
            float n1_k = norm_k(1,h4-h1,0,h2-h1);
            float mag1 = magnitude(n1_i,n1_j,n1_k);
            n1_i = -n1_i/mag1;
            n1_j = -n1_j/mag1;
            n1_k = -n1_k/mag1;
            
            // u1,u2,u3 // 0,h1-h2,-1 // // v1,v2,v3 // 1,h3-h2,0
            float n2_i = norm_i(h1-h2,-1,h3-h2,0);
            float n2_j = norm_j(0,-1,1,0);
            float n2_k = norm_k(0,h1-h2,1,h3-h2);
            float mag2 = magnitude(n2_i,n2_j,n2_k);
            n2_i = -n2_i/mag2;
            n2_j = -n2_j/mag2;
            n2_k = -n2_k/mag2;
            
            // u1,u2,u3 // -1,h2-h3,0 // // v1,v2,v3 // 0,h4-h3,-1
            float n3_i = norm_i(h2-h3,0,h4-h3,-1);
            float n3_j = norm_j(-1,0,0,-1);
            float n3_k = norm_k(-1,h2-h3,0,h4-h3);
            float mag3 = magnitude(n3_i,n3_j,n3_k);
            n3_i = -n3_i/mag3;
            n3_j = -n3_j/mag3;
            n3_k = -n3_k/mag3;
            
            // u1,u2,u3 // 0,h3-h4,1 // // v1,v2,v3 // -1,h1-h4,0
            float n4_i = norm_i(h3-h4,1,h1-h4,0);
            float n4_j = norm_j(0,1,-1,0);
            float n4_k = norm_k(0,h3-h4,-1,h1-h4);
            float mag4 = magnitude(n4_i,n4_j,n4_k);
            n4_i = -n4_i/mag4;
            n4_j = -n4_j/mag4;
            n4_k = -n4_k/mag4;
            
            // Colour values based on the height
            float diff = (maxHeight - minHeight);
            float c1 = h1/diff;
            float c2 = h2/diff;
            float c3 = h3/diff;
            float c4 = h4/diff;
            
            if(Mode == 3){ //colour wireframe black
                c1 = 0;
                c2 = 0;
                c3 = 0;
            }
            
            glBegin(GL_QUADS);
            
            glColor3f(c1, c1, c1);
            if (lightSwitch==1){glNormal3f(n1_i,n1_j,n1_k);}
            glVertex3f(x,h1,z);
            
            glColor3f(c2, c2, c2);
            if (lightSwitch==1){glNormal3f(n2_i,n2_j,n2_k);}
            glVertex3f(x,h2,z+1);
            
            glColor3f(c3, c3, c3);
            if (lightSwitch==1){glNormal3f(n3_i,n3_j,n3_k);}
            glVertex3f(x+1,h3,z+1);
            
            glColor3f(c4, c4, c4);
            if (lightSwitch==1){glNormal3f(n4_i,n4_j,n4_k);}
            glVertex3f(x+1,h4,z);
    
            glEnd();
            
            z++;
        }
        z = 0;
        x++;
    }
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //lighting
    if (lightSwitch == 1){
        glLightfv(GL_LIGHT0, GL_POSITION, pos0);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
        glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);
        
        glLightfv(GL_LIGHT1, GL_POSITION, pos1);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, diff1);
        glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);
    }

    //material
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);
    
    gluLookAt(eye[0], eye[1], eye[2], look[0], look[1], look[2], 0, 1, 0);
    
    if(Mode == 1){  // Regular mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawTerrain();
    }
    else if(Mode == 2){     // Wireframe
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawTerrain();
    }
    else if(Mode == 3){  // Draw the regular and wireframe terrain
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawTerrain();
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawTerrain();
        glEnable(GL_LIGHTING);
    }
    
    glutSwapBuffers();
    glutPostRedisplay();
}

void display2(){    // Second display for the map
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_SHADE_MODEL);
    
    gluOrtho2D(0, size, 0, size);
    
    int x = 0;
    int z = 0;
    
    while(x <= size){
        while(z <= size){
            float h = heights[x][z];
            float diff = (maxHeight - minHeight);
            float c = h/diff;
            
            glBegin(GL_POLYGON);
            glColor3f(c, c, c);
            glVertex2f(x, z);
            glVertex2f(x, z+1);
            glVertex2f(x+1, z+1);
            glVertex2f(x+1, z);
            glEnd();
            z++;
            
        }
        z = 0;
        x++;
    }
    
    glutSwapBuffers();
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y){
    if(key == 'Q' || key == 'q')
    {
        exit(0);
    }
    if(key == 'W' || key == 'w')    // Toggle view mode
    {
        Mode++;
        if(Mode == 4){
            Mode = 1;
        }
    }
    if(key == 'L' || key == 'l')    // Toggle Lights
    {
        if(lightSwitch == 1){
            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
            glDisable(GL_LIGHT1);
            lightSwitch = 0;
        }
        else if(lightSwitch == 0){
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
            glEnable(GL_LIGHT1);
            
            lightSwitch = 1;
        }
    }
    
    if(key == 'S' || key == 's')    // Toggle Shading
    {
        if (shadeSwitch==1){
            glShadeModel(GL_FLAT);
            shadeSwitch = 0;
        }
        else if (shadeSwitch==0){
            glShadeModel(GL_SMOOTH);
            shadeSwitch = 1;
        }
    }

    if(key == 'R' || key == 'r')    // Reset Terrain
    {
        mapReset();
        maxHeight = 0;
        heightmap();
    }
    if(key == 'Z' || key == 'z')
    {
        pos0[0]+=3;
    }
    if(key == 'X' || key == 'x')
    {
        pos0[0]-=3;
    }
    if(key == 'C' || key == 'c')
    {
        pos1[0]+=4;
    }
    if(key == 'V' || key == 'v')
    {
        pos1[0]-=4;
    }
}

void special(int key, int x, int y){    // Camera Control
    switch(key){
        case GLUT_KEY_LEFT:
            yCameraLeft();
            break;
        case GLUT_KEY_RIGHT:
            yCameraRight();
            break;
        case GLUT_KEY_UP:
            xzCameraDown();
            break;
        case GLUT_KEY_DOWN:
            xzCameraUp();
            break;
    }
}

void mouse(int btn, int state, int x, int y){
    //printf("mouse: %i,%i\n", x, y);
    switch(btn){
        case GLUT_LEFT_BUTTON:
        {
            heightUp(x,y);  //displace terrain up
            break;
        }
        case GLUT_RIGHT_BUTTON:
        {
            heightDown(x,y);    //displace terrain down
            break;
        }
    }
}

void menu_1(int value){     // Menu for selecting terrain sizes, 100 is the default
    switch(value){
        case 0:
            size = 50;
            mapReset();
            heightmap();
            break;
        case 1:
            size = 75;
            mapReset();
            heightmap();
            break;
        case 2:
            size = 100;
            mapReset();
            heightmap();
            break;
        case 3:
            size = 150;
            mapReset();
            heightmap();
            break;
        case 4:
            size = 300;
            mapReset();
            heightmap();
            break;
    }
}

void initMenu(){
    int size_menu = glutCreateMenu(menu_1);
    
    //Size Menu
    glutSetMenu(size_menu);
    glutAddMenuEntry("Size 50",0);
    glutAddMenuEntry("Size 75",1);
    glutAddMenuEntry("Size 100 (default)",2);
    glutAddMenuEntry("Size 150",3);
    glutAddMenuEntry("Size 300",4);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
}


void glutCallbacks1(){
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
}

void glutCallbacks2(){
    glutDisplayFunc(display2);
    glutMouseFunc(mouse);
    glutSpecialFunc(special);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    
    // create window 2 //
    // Referenced this: http://www.codeincodeblock.com/2012/05/creating-multiple-windows-with-opengl.html
    glutInitWindowSize(300, 300);
    glutCreateWindow("MAP");
    glutPositionWindow(650, 35);
    glutCallbacks2();
    
    //create window 1
    glutInitWindowSize(600, 600);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("TERRAIN");
    
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    //glClearColor(1, 1, 1, 0);
    gluPerspective(45,1,1,100);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    
    
    printf("TERRAIN CONTROLS\nQ: Quit\nR: Reset Map\nW: Toggle Wireframe\nS: Toggle Shading\nRight Click: Size Menu\nArrow Keys: Turn Camera\nZ: Move Light1 +x\nX: Move Light1 -x\nC: Move Light2 +x\nV: Move Light2 -x\n");
    printf("\nMAP CONTROLS\nLeft Click Displace Terrain Upwards\nRight Click: Displace Terrain Downwards\n");
    
    heightmap(); //load the heightmap with values
    
    glutCallbacks1();
    
    initMenu();
    
    glutMainLoop();
    
    return(0);
}
