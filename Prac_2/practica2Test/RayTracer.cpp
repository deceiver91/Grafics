#include "RayTracer.h"

// Metode per netejar la memòria

void cleanup() {
    delete scene;
}


// Metode Render

// Aquest metode pinta pixels en la finestra GL usant GL_POINT. Es crida cada vegada que cal refrescar la pantalla

// Aquesta funcio ha de transformar cada pixel a coordenades de mon i
// envia un raig des de la camera en aquella direccio cridant al metode CastRay
// i pel pintar la imatge final:
// 1) Neteja la pantalla per a visualitzar el nou frame
// 2) Envia un raig a l'escena per a cada pixel de la pantalla i utilitza el color retornat per a pintar el pixel
// 3) Actualitza la finestra GL fent flush de les intruccions GL


void Render()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// Netejar la finestra OpenGL

    // TODO: Cridar a IniViewProjMatrices de la classe camera (punt 2 de l'enunciat)
    scene->cam->IniViewProjMatrices();

    glBegin(GL_POINTS);	//S'activa el mode GL_POINTS. En aquest mode
                        // cada vertex especificat és un punt.
                        //	Reference https://en.wikibooks.org/wiki/OpenGL_Programming/GLStart/Tut3 si us interessa.

    std::cout<<"observador:"<<scene->cam->obs.x<<","<<scene->cam->obs.y<<","<<scene->cam->obs.z<<std::endl;

	glm::mat4 viewMatrixInverse = glm::inverse(scene->cam->viewMatrix);
	glm::mat4 projMatrixInverse = glm::inverse(scene->cam->projMatrix);
	glm::mat4 viewCrossProj = viewMatrixInverse * projMatrixInverse;

    // Recorregut de cada pixel de la imatge final
    for(int x = 0; x < scene->cam->viewportX; ++x)
        for(int y = 0; y < scene->cam->viewportY; ++y){
			/*
			 *De pixel (viewport) a window normalizado
			 */
            float pixelX =  2*((x+0.5f)/scene->cam->viewportX)-1; //coords de camera a partir de viewport
            float pixelY = -2*((y+0.5f)/scene->cam->viewportY)+1;


            // TODO: A canviar en el punt 3 de la pràctica. Ara s'esta suposant que l'observador està situat al punt
            // 0,0,10 i s'està considerant que el angle d'obertura es de arcos (1/10) i la relació de l'amplada i l'alçada
            // es 1. Aixi, sota aquests suposits, les coordenades de window es poden calcular amb el calcul
            // de pixelX, pixelY i les coordenades mon i de camera es poden considerar les mateixes.

            // TODO: Cal canviar aquestes 2 linies per a fer la transformacio de pixel a coordenades de mon de forma correcta
            // en qualsevol transformacio perspectiva

            // glm::vec3 pixelPosWorld = glm::vec3(pixelX, pixelY, 0.0f);
            // glm::vec3 direction = glm::normalize(glm::vec3(pixelPosWorld-scene->cam->obs));

			/*
			 * Obtenemos los pixeles para znear y zfar			 *
			 */
			glm::vec4 pixelPosWorldZnear = glm::vec4(pixelX, pixelY, -1.0f, 1.0f);
			glm::vec4 pixelPosWorldZfar = glm::vec4(pixelX, pixelY, 1.0f, 1.0f);

			glm::vec4 worldZnear = viewCrossProj * pixelPosWorldZnear;
			glm::vec4 worldZfar = viewCrossProj * pixelPosWorldZfar;

			glm::vec4 direction = glm::normalize(glm::vec4( (worldZfar/worldZfar.w)-(worldZnear/worldZnear.w)));

            Payload payload;
            // Creacio del raig
            // HELP: Ray(const glm::vec3 &origin, const glm::vec3 &direction)
            // Ray ray(glm::vec3(pixelPosWorldZnear.x, pixelPosWorldZnear.y, pixelPosWorldZnear.z), glm::vec3(direction.x, direction.y, direction.z)) ;
            Ray ray(scene->cam->obs, glm::vec3(direction.x, direction.y, direction.z)) ;
            // Ray ray(scene->cam->obs, direction) ;

            if(scene->CastRay(ray,payload) > 0.0f){
				glColor3f(payload.color.x,payload.color.y,payload.color.z);
			}
			else {
                // TODO: A canviar per la Intensitat ambient global
                glColor3f(0.2f,0.22f,0.25f);
			}

			glVertex3f(pixelX,pixelY,0.0f);
		}

	glEnd();
	glFlush();
}

// Metode principal del programa

int main(int argc, char **argv) {

    // inicialitza l'escena: el constructor de l'escena s'encarrega de
    // crear la camera, els objectes i les llums
    scene = new Scene();

    //inicialitza OpenGL
	glutInit(&argc, argv);
    //Define la mida de la window GL especificada per la mida del viewport
    glutInitWindowSize(scene->cam->viewportX, scene->cam->viewportY);

    //Creacio de la window per pintar amb GL
	glutCreateWindow("RayTracer");
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    //Posa la funcio Render per a que la window GL cridi quan es refresca
    glutDisplayFunc(Render); // El equivalente al GLPaint()

    //	TODO: Afegir els objectes a l'escena
    //  TODO: Afegir les llums a l'escena

	atexit(cleanup);

    // Inicia el main loop per a poder refrescar l'escena
	glutMainLoop();
}
