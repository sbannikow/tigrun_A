#include "hMoving.h"



void firstPersonMovingCalc(GLFWwindow* window, glm::vec3 &out_camDir, GLfloat &out_angleX, GLfloat &t_WX, GLfloat &t_WY)
{
        GLdouble wX = 0, wY = 0, deltaX = 0, deltaY = 0, t_wX = 0, t_wY = 0;

        GLint WIDTH = 0, HEIGHT = 0;

        glfwGetWindowSize(window, &WIDTH, &HEIGHT);
        t_wX = WIDTH/2;
        t_wY = HEIGHT/2;



        glfwGetCursorPos(window, &wX, &wY);


        deltaX = wX - t_wX;
        deltaY = t_wY - wY;
        t_WX = t_WX + deltaX*0.08;
        t_WY = t_WY + deltaY*0.08;






        if(t_WY<182) t_WY=182;
        if(t_WY>358) t_WY=358;


        out_angleX = t_WX;
        out_camDir.z = sin(glm::radians(t_WY))*sin(glm::radians(t_WX));
        out_camDir.x = cos(glm::radians(t_WX))*sin(glm::radians(t_WY));
        out_camDir.y = cos(glm::radians(t_WY));




    /*                if(iKey['W']) {out_camPos.z-= sin(glm::radians(WX))*speed; out_camPos.x -=cos(glm::radians(WX))*speed;}
                    if(iKey['S']) {out_camPos.z+= sin(glm::radians(WX))*speed; out_camPos.x +=cos(glm::radians(WX))*speed;}
                    if(iKey['A']) out_camPos-= glm::normalize(glm::cross(out_camDir, camUp))*speed;
                    if(iKey['D']) out_camPos+= glm::normalize(glm::cross(out_camDir, camUp))*speed;*/

          /*if(speed!=0){
                       // cz+= sin(cAngle)*speed;
                        //cx+= cos(cAngle)*speed;
                        zZ += sin(angleY)*sin(angleX)*speed;
                        xX += cos(angleX)*sin(angleY)*speed;
                        printf("%f:%f\n", angleX, zZ);
                        }*/


        glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);
}

void firstPersonMoveForward (GLfloat in_angleX, glm::vec3 &out_camPos, GLfloat speed)
{
    out_camPos.z -= sin(glm::radians(in_angleX))*speed;
    out_camPos.x -= cos(glm::radians(in_angleX))*speed;
}

void firstPersonMoveBackward (GLfloat in_angleX, glm::vec3 &out_camPos, GLfloat speed)
{
    out_camPos.z += sin(glm::radians(in_angleX))*speed;
    out_camPos.x += cos(glm::radians(in_angleX))*speed;
}

void firstPersonMoveSEward(GLfloat in_angleX, glm::vec3 &out_camPos, GLfloat speed)
{
    GLfloat modAngle;
    modAngle = glm::radians(in_angleX)-M_PI*-0.25;
    out_camPos.z += sin(glm::radians(modAngle))*speed;
    out_camPos.x += cos(glm::radians(modAngle))*speed;
}

void firstPersonStrafeLeft (glm::vec3 in_camDir, glm::vec3 in_camUp, glm::vec3 &out_camPos, GLfloat speed)
{
    out_camPos -= glm::normalize(glm::cross(in_camDir, in_camUp))*speed;

}




void firstPersonStrafeRight (glm::vec3 in_camDir, glm::vec3 in_camUp, glm::vec3 &out_camPos, GLfloat speed)
{
    out_camPos += glm::normalize(glm::cross(in_camDir, in_camUp))*speed;

}

