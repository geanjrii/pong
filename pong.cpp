#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <SFML/Audio.hpp>

/////////////////////////////////
struct
{
    float x, y;
} ball;

/* bool hit[256]; */

float center_racket1, center_racket2;
int score1 = 0, score2 = 0;
int height = 720, width = 720;

// dx e dy controlam a velocidade da bola
float dx = -.002, dy = 0.002;
float epsilon = 0.001;

char gameOverMsg[] = "Game over! Deseja reiniciar? (Y/N)";
bool showGameOverDialog = false;

bool spacePressed = true;
bool ball_moving = true;
bool pontuacaoP1 = false;
bool pontuacaoP2 = false;





///////////////////////////////////

sf::SoundBuffer buffer;
sf::Sound sound;
void loadSound(std::string soundFilePath)
{
    if (!buffer.loadFromFile(soundFilePath))
    {
        exit(0);
    }
    else
    {
        sound.setBuffer(buffer);
    }
}

///////////////////////////////////

// Essa função é responsável por desenhar as raquetes na tela.
void draw_racket()
{
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(-.05 - 1, .2 + center_racket1);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2f(.05 - 1, .2 + center_racket1);
    glColor3f(0.0, 0.0, 1.0);
    glVertex2f(.05 - 1, -.2 + center_racket1);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(-.05 - 1, -.2 + center_racket1);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(-.05 + 1, .2 + center_racket2);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2f(.05 + 1, .2 + center_racket2);
    glColor3f(0.0, 0.0, 1.0);
    glVertex2f(.05 + 1, -.2 + center_racket2);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(-.05 + 1, -.2 + center_racket2);
    glEnd();
}

// Essa função é responsável por desenhar a bola na tela.
void draw_ball(void)
{
    glPushMatrix();
    glTranslatef(ball.x, ball.y, 0);
    glColor3f(1, 1, 0);
    gluDisk(gluNewQuadric(), 0, 0.04, 32, 1);
    glPopMatrix();
}


// Essa função é responsável por verificar se a bola bateu nas paredes ou nas raquetes, e ajustar sua posição e direção em conformidade.
void rebounds(void)
{
    // Reiniciar velocidade da bola ao pontuar
    if (ball.x > 1 || ball.x < -1)
    {
        dx = -.002;
        dy = 0.002;
    }

    // saltar para cima
    if (ball.y > 1)
    {
        ball.y = 1;
        dy = -dy;
    }
    // saltar para baixo
    if (ball.y < -1)
    {
        ball.y = -1;
        dy = -dy;
    }

    // salto para direita
    if ((ball.x > .9) && (ball.y > -.2 + center_racket2) && (ball.y < .2 + center_racket2))
    {
        loadSound("hit.wav");
        sound.play();

        ball.x = .9;
        dx = -dx;
        dy += (ball.y - center_racket2) / 500;
        dx -= epsilon; // adiciona epsilon à velocidade atual na direção x
        dy -= epsilon; // adiciona epsilon à velocidade atual na direção y
    }

    if (ball.x > 1)
    {
        loadSound("scored.wav");
        sound.play();
        center_racket1 = 0;
        center_racket2 = 0;
        ball_moving = false;
        ball.x = -0.9;
        ball.y = center_racket1;
        dx = -dx;
        score1++;
        pontuacaoP2 = false;
        pontuacaoP1 = true;
    }
    // exit(0);

    // salto para esquerda
    if ((ball.x < -.9) && (ball.y > -.2 + center_racket1) && (ball.y < .2 + center_racket1))
    {
        loadSound("hit.wav");
        sound.play();

        ball.x = -.9;
        dx = -dx;
        dy += (ball.y - center_racket1) / 500;
        dx += epsilon; // adiciona epsilon à velocidade atual na direção x
        dy += epsilon; // adiciona epsilon à velocidade atual na direção y
    }

    if (ball.x < -1)
    {
        loadSound("scored.wav");
        sound.play();
        center_racket1 = 0;
        center_racket2 = 0;
        ball_moving = false;
        ball.x = 0.9;
        ball.y = center_racket2;
        dx = +dx;
        score2++;
        pontuacaoP2 = true;
        pontuacaoP1 = false;
    }
    // exit(0);
}

// Função de callback para o evento de teclado
void keyboardCallback(unsigned char key, int x, int y)
{
    if (showGameOverDialog)
    {
        if (toupper(key) == 'Y')
        {
            // Reiniciar o jogo
            ball.x = 0;
            ball.y = 0;
            dx = -.002;
            dy = 0.002;
            score1 = 0;
            score2 = 0;
            showGameOverDialog = false;
        }
        else if (toupper(key) == 'N')
        {
            // Sair do jogo
            exit(0);
        }
    }
}

void display(void)
{
    if (score1 == 15 || score2 == 15)
    {
        showGameOverDialog = true;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_LINE_STIPPLE);   // Habilita o pontilhamento da linha
    glLineStipple(1, 0xAAAA);    // Define o padrão do pontilhamento
    glBegin(GL_LINES);           // Define que será desenhada uma linha
    glColor3f(1.0f, 1.0f, 1.0f); // Define a cor da linha como branca
    glVertex2f(0.0f, -1.0f);     // Define o primeiro ponto da linha
    glVertex2f(0.0f, 1.0f);      // Define o segundo ponto da linha
    glEnd();                     // Finaliza o desenho da linha
    glDisable(GL_LINE_STIPPLE);  //
    if (showGameOverDialog)
    {

        // Renderizar mensagem de vencedor
        if (score1 > score2)
        {
            glRasterPos2f(-0.4f, -0.1f);
            for (int i = 0; i < strlen("Jogador 1 venceu!"); i++)
            {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, "Jogador 1 venceu!"[i]);
            }
        }
        else if (score2 > score1)
        {
            glRasterPos2f(-0.4f, -0.1f);
            for (int i = 0; i < strlen("Jogador 2 venceu!"); i++)
            {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, "Jogador 2 venceu!"[i]);
            }
        }

        // Desenhar a janela de diálogo
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(-0.5f, 0.0f);
        for (int i = 0; i < strlen(gameOverMsg); i++)
        {
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, gameOverMsg[i]);
        }
    }

    if (!showGameOverDialog && spacePressed)
    {
        draw_racket();

        if (ball_moving)
        {
            // atualize a posição da bola e verifique se ela bateu nas paredes ou raquetes
            ball.x += dx;
            ball.y += dy;
            rebounds();
        }
        draw_ball();

        glVertex2f(-.04 + ball.x + center_racket1, -.04 + ball.y);
        glVertex2f(-.04 + ball.x + center_racket1, .04 + ball.y);
        glVertex2f(.04 + ball.x + center_racket1, .04 + ball.y);
        glVertex2f(.04 + ball.x + center_racket1, -.04 + ball.y);

        // Exibir a pontuação na tela
        char score[50];
        sprintf(score, "Player 1: %d     Player 2: %d", score1, score2);

        // calcular a largura do texto
        int textWidth = 0;
        for (int i = 0; i < strlen(score); i++)
        {
            textWidth += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, score[i]);
        }

        // centralizar o texto
        float xPos = -0.7f + (1.0f - (float)textWidth / glutGet(GLUT_WINDOW_WIDTH)) / 2.0f;

        // renderizar o texto centralizado
        glRasterPos2f(xPos, 0.9f);
        for (int i = 0; i < strlen(score); i++)
        {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, score[i]);
        }

        /* // Exibir a pontuação na tela
        char score[50];
        sprintf(score, "Player 1: %d     Player 2: %d", score1, score2);
        glRasterPos2f(-0.8f, 0.9f);
        for (int i = 0; i < strlen(score); i++)
        {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, score[i]);
        } */
    }
    else if (!showGameOverDialog)
    {
        pontuacaoP1 = false;
        pontuacaoP2 = false;
        draw_racket();
        draw_ball();
        char score[50];
        sprintf(score, "Jogo Pausado");
        // glScalef(2.0, 2.0, 2.0);
        glRasterPos2f(-0.1f, 0.2f);
        for (int i = 0; i < strlen(score); i++)
        {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, score[i]);
            // glTranslatef(10.0, 0.0, 0.0);
        }
    }
    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();
}

// Função para controlar a raquete direita com as setas
void specialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        if (center_racket2 < 1 && spacePressed && ball_moving)
        {
            center_racket2 += .0900;
        }
        else if (pontuacaoP2 && center_racket2 < 1)
        {
            center_racket2 += .0900;
            ball.y += .0900;
        }
        break;
    case GLUT_KEY_DOWN:
        if (center_racket2 > -1 && spacePressed && ball_moving)
        {
            center_racket2 -= .0900;
        }
        else if (pontuacaoP2 && center_racket2 > -1)
        {
            center_racket2 -= .0900;
            ball.y -= .0900;
        }
        break;
    }

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{

    if (showGameOverDialog)
    {
        if (toupper(key) == 'Y')
        {
            // Reiniciar o jogo
            ball.x = 0;
            ball.y = 0;
            dx = -.002;
            dy = 0.002;
            score1 = 0;
            score2 = 0;
            showGameOverDialog = false;
        }
        else if (toupper(key) == 'N')
        {
            // Sair do jogo
            exit(0);
        }
    }

    if (key == 'w')
    {
        if (center_racket1 < 1 && spacePressed && ball_moving)
        {
            center_racket1 += .09;
        }

        else if (pontuacaoP1 && center_racket1 < 1)
        {
            center_racket1 += .09;
            ball.y += .0900;
        }
    }
    else if (key == 's')
    {
        if (center_racket1 > -1 && spacePressed && ball_moving)
        {
            center_racket1 -= .09;
        }
        else if (pontuacaoP1 && center_racket1 > -1)
        {
            center_racket1 -= .09;
            ball.y -= .0900;
        }
    }
    else if (key == 13)
    {
        ball_moving = true;
        pontuacaoP1 = false;
        pontuacaoP2 = false;
    }
    else if (key == ' ')
    {
        // Inverter o estado da variável spacePressed
        spacePressed = !spacePressed;
    }
}



///////////////////////////////////////////////
int main(int argc, char **argv)
{
    glutInitWindowSize(width, height);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutCreateWindow("pong");
    glutPositionWindow((glutGet(GLUT_SCREEN_WIDTH) - width) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - height) / 2);

    glutSetCursor(GLUT_CURSOR_NONE);

    glClearColor(0.0, 0.0, 0.0, 0.0);

    glMatrixMode(GL_PROJECTION);
    glOrtho(-1, 1, -1, 1, 0, 1);
    glMatrixMode(GL_MODELVIEW);

    ball.x = ball.y = 0;
    center_racket1 = center_racket2 = 0;

    // callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
    return 0;
}
