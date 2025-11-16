#define GLEW_STATIC
#include "../Graphics/include/glew.h"
#include "../Graphics/include/freeglut.h"

#include "Renderer.h"
#include "Camera.h"
#include <chrono>
#include <thread>
#include <string>


static Game* gptr = nullptr;
static Camera cam;

static bool dragging = false;
static int lastX = 0, lastY = 0;

static void applyCamera() {
    glScalef(cam.zoom, cam.zoom, 1.0f);
    glTranslatef(-cam.x, -cam.y, 0.0f);
}

static void drawQuad(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

static void drawText(float x, float y, const std::string& s) {
    glColor3f(0, 0, 0);
    glRasterPos2f(x, y);
    for (unsigned char c : s)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
}

static void display() {
    auto& game = *gptr;

    glClearColor(0.85f, 1.0f, 0.85f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    applyCamera();

    float cw = 1.0f;
    float ch = 1.0f;

    // map
    for (int y = 0; y < game.grid.h; ++y) {
        for (int x = 0; x < game.grid.w; ++x) {
            auto t = game.grid.cells[y * game.grid.w + x];
            float X = x * cw;
            float Y = y * ch;

            switch (t) {
            case Tile::Rock:       drawQuad(X, Y, cw, ch, 0.5, 0.5, 0.5); break;
            case Tile::Tree:       drawQuad(X, Y, cw, ch, 0.1, 0.5, 0.1); break;
            case Tile::Water:      drawQuad(X, Y, cw, ch, 0.1, 0.3, 0.8); break;
            case Tile::DepotAmmo:  drawQuad(X, Y, cw, ch, 0.9, 0.9, 0.2); break;
            case Tile::DepotMed:   drawQuad(X, Y, cw, ch, 0.9, 0.9, 0.2); break;
            default:               drawQuad(X, Y, cw, ch, 0.92, 1.0, 0.92);
            }
        }
    }

    // agents
    auto drawA = [&](const Agent& a, float r, float g, float b) {
        if (!a.alive) return;

        float X = a.pos.x;
        float Y = a.pos.y;

        drawQuad(X, Y, cw, ch, r, g, b);
        drawText(X + 0.3f, Y + 0.6f, std::string(1, a.glyph));
        drawText(X + 0.1f, Y + 1.05f, std::to_string(a.hp));
        };

    for (auto& A : { &game.blue.commander })
        drawA(*A, 0.1f, 0.3f, 1.0f);
    for (auto& w : game.blue.warriors)
        drawA(w, 0.3f, 0.5f, 1.0f);
    drawA(game.blue.medic, 0.1f, 0.6f, 1.0f);
    drawA(game.blue.porter, 0.1f, 0.6f, 1.0f);

    for (auto& A : { &game.orange.commander })
        drawA(*A, 1.0f, 0.4f, 0.0f);
    for (auto& w : game.orange.warriors)
        drawA(w, 1.0f, 0.6f, 0.1f);
    drawA(game.orange.medic, 1.0f, 0.7f, 0.2f);
    drawA(game.orange.porter, 1.0f, 0.7f, 0.2f);
    // ----- Bullets -----
    for (const auto& b : game.bullets.bullets)
    {
        if (!b.alive) continue;
        float size = 0.3f;
        drawQuad(b.x - size * 0.5f,
            b.y - size * 0.5f,
            size, size,
            1.0f, 1.0f, 0.0f); // צהוב קטן
    }
    // ----- TRIAL -----
    for (const auto& b : game.bullets.bullets)
    {
        for (const auto& t : b.trail)
        {
            float alpha = t.life;
            glColor3f(1.0f * alpha, 0.8f * alpha, 0.0f);
            drawQuad(t.x, t.y, 0.15f, 0.15f, 1, 0.6, 0.0);
        }
    }

    // ----- BULLET -----
    for (const auto& b : game.bullets.bullets)
    {
        glColor3f(1.0, 1.0, 0.0); // צהוב
        drawQuad(b.x, b.y, 0.3f, 0.3f, 1, 1, 0);
    }

    // ----- GRENADES -----
    for (const auto& g : game.grenades.grenades )
    {
        glColor3f(1.0, 0.3, 0.0);
        drawQuad(g.x, g.y, 0.4f, 0.4f, 1, 0.3, 0.0);
    }


    glutSwapBuffers();
}

static void mouseWheel(int wheel, int dir, int x, int y) {
    cam.addZoom(dir * 0.1f);
    glutPostRedisplay();
}

static void mouseDown(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            dragging = true;
            lastX = x;
            lastY = y;
        }
        else {
            dragging = false;
        }
    }
}

static void mouseMove(int x, int y) {
    if (dragging) {
        cam.pan((lastX - x) * 0.02f, (y - lastY) * 0.02f);
        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

static void timer(int) {
    gptr->step();
    glutPostRedisplay();
    glutTimerFunc(33, timer, 0);
}

void runGraphics(Game& game) {
    gptr = &game;

    int argc = 1;
    char* argv[1] = { (char*)"app" };
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("AI Battle - Camera System");

    glewInit();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, game.grid.w, game.grid.h, 0, -1, 1);

    glutDisplayFunc(display);
    glutMouseFunc(mouseDown);
    glutMotionFunc(mouseMove);
    glutMouseWheelFunc(mouseWheel);

    glutTimerFunc(33, timer, 0);
    glutMainLoop();
}
