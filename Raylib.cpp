#include <raylib.h>
#include <math.h>
#include <stdio.h>

// Define constants for the simulation
#define GRID_SIZE 150                     // Size of the grid (100x100)
#define SCREEN_WIDTH 600                  // Width of the screen
#define SCREEN_HEIGHT 600                 // Height of the screen
#define CELL_SIZE (SCREEN_WIDTH / GRID_SIZE) // Size of each cell in the grid
#define DIFFUSION_RATE 0.1f               // Rate at which density diffuses
#define VISCOSITY 0.0001f                 // Viscosity of the fluid
#define DT 0.1f                           // Time step for the simulation

// Define a struct to represent a particle in the grid
typedef struct {
    Vector2 position;       // Position of the particle in the grid
    Vector2 velocity;       // Velocity of the particle
    float density;          // Density of the particle
    Color color;            // Color representation of the particle
} Eulerian_Particle;

// Declare global variables for the simulation
Eulerian_Particle particles[GRID_SIZE][GRID_SIZE]; // 2D array of particles
float temp[GRID_SIZE][GRID_SIZE];                  // Temporary array for calculations
Vector2 mouse;                                     // Store mouse position for interaction

// Function to initialize particles in the grid
void InitializeParticles() {
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int y = 0; y < GRID_SIZE; y++) {
            particles[x][y].position.x = x * CELL_SIZE; // Set particle position
            particles[x][y].position.y = y * CELL_SIZE;
            particles[x][y].velocity.x = 0;            // Initialize velocity to 0
            particles[x][y].velocity.y = 0;
            particles[x][y].density = GetRandomValue(1, 10); // Random density
            particles[x][y].color = { 0, 0, 0, 255 };  // Default color (black)
        }
    }
}

// Function to copy data from the temporary array back to the particles
void Copy_Back(int property) {
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int y = 0; y < GRID_SIZE; y++) {
            if (property == 0) {
                particles[x][y].velocity.x = temp[x][y]; // Copy velocityX
            }
            else if (property == 1) {
                particles[x][y].velocity.y = temp[x][y]; // Copy velocityY
            }
            else if (property == 2) {
                particles[x][y].density = temp[x][y];   // Copy density
            }
        }
    }
}

// Function to diffuse a property (velocity or density) across the grid
void Diffuse(int property, float rate) {
    for (int x = 1; x < GRID_SIZE - 1; x++) {
        for (int y = 1; y < GRID_SIZE - 1; y++) {
            float value;
            if (property == 0) { // Diffuse velocityX
                value = particles[x][y].velocity.x;
                temp[x][y] = value + rate * (
                    particles[x - 1][y].velocity.x + particles[x + 1][y].velocity.x +
                    particles[x][y - 1].velocity.x + particles[x][y + 1].velocity.x - 4 * value
                    );
            }
            else if (property == 1) { // Diffuse velocityY
                value = particles[x][y].velocity.y;
                temp[x][y] = value + rate * (
                    particles[x - 1][y].velocity.y + particles[x + 1][y].velocity.y +
                    particles[x][y - 1].velocity.y + particles[x][y + 1].velocity.y - 4 * value
                    );
            }
            else if (property == 2) { // Diffuse density
                value = particles[x][y].density;
                temp[x][y] = value + rate * (
                    particles[x - 1][y].density + particles[x + 1][y].density +
                    particles[x][y - 1].density + particles[x][y + 1].density - 4 * value
                    );
            }
        }
    }

    // Copy the diffused values back to the particles
    Copy_Back(property);
}

// Function to advect a property (velocity or density) based on the fluid flow
void Advect(int property, float dt) {
    for (int x = 1; x < GRID_SIZE - 1; x++) {
        for (int y = 1; y < GRID_SIZE - 1; y++) {
            float prevX = x - dt * particles[x][y].velocity.x / CELL_SIZE;
            float prevY = y - dt * particles[x][y].velocity.y / CELL_SIZE;

            // Clamp to grid boundaries to prevent out-of-bounds access
            prevX = fmax(0, fmin(GRID_SIZE - 1.001, prevX));
            prevY = fmax(0, fmin(GRID_SIZE - 1.001, prevY));

            int i0 = (int)prevX;
            int j0 = (int)prevY;
            int i1 = i0 + 1;
            int j1 = j0 + 1;

            float s1 = prevX - i0;
            float t1 = prevY - j0;
            float s0 = 1 - s1;
            float t0 = 1 - t1;

            if (property == 0) { // Advect velocityX
                temp[x][y] = s0 * (t0 * particles[i0][j0].velocity.x + t1 * particles[i0][j1].velocity.x) +
                    s1 * (t0 * particles[i1][j0].velocity.x + t1 * particles[i1][j1].velocity.x);
            }
            else if (property == 1) { // Advect velocityY
                temp[x][y] = s0 * (t0 * particles[i0][j0].velocity.y + t1 * particles[i0][j1].velocity.y) +
                    s1 * (t0 * particles[i1][j0].velocity.y + t1 * particles[i1][j1].velocity.y);
            }
            else if (property == 2) { // Advect density
                temp[x][y] = s0 * (t0 * particles[i0][j0].density + t1 * particles[i0][j1].density) +
                    s1 * (t0 * particles[i1][j0].density + t1 * particles[i1][j1].density);
            }
        }
    }

    Copy_Back(property);
}

// Function to project the velocity field to make it incompressible
void Project() {
    float div[GRID_SIZE][GRID_SIZE] = { 0 }; // Divergence of the velocity field
    float p[GRID_SIZE][GRID_SIZE] = { 0 };   // Pressure field

    // Compute divergence of the velocity field
    for (int x = 1; x < GRID_SIZE - 1; x++) {
        for (int y = 1; y < GRID_SIZE - 1; y++) {
            div[x][y] = -0.5f * CELL_SIZE * (
                particles[x + 1][y].velocity.x - particles[x - 1][y].velocity.x +
                particles[x][y + 1].velocity.y - particles[x][y - 1].velocity.y
                );
            p[x][y] = 0;
        }
    }

    // Solve for pressure using Gauss-Seidel iteration
    for (int k = 0; k < 20; k++) {
        for (int x = 1; x < GRID_SIZE - 1; x++) {
            for (int y = 1; y < GRID_SIZE - 1; y++) {
                p[x][y] = (div[x][y] + p[x - 1][y] + p[x + 1][y] + p[x][y - 1] + p[x][y + 1]) / 4.0f;
            }
        }
    }

    // Subtract the gradient of pressure from the velocity field
    for (int x = 1; x < GRID_SIZE - 1; x++) {
        for (int y = 1; y < GRID_SIZE - 1; y++) {
            particles[x][y].velocity.x -= 0.5f * (p[x + 1][y] - p[x - 1][y]) / CELL_SIZE;
            particles[x][y].velocity.y -= 0.5f * (p[x][y + 1] - p[x][y - 1]) / CELL_SIZE;
        }
    }
}

// Function to enforce boundary conditions on the velocity field
void SetBoundaries(int property) {
    if (property == 0) {
        for (int x = 0; x < GRID_SIZE; x++) {
            particles[x][0].velocity.y = -particles[x][1].velocity.y; // Reflect velocityY at the top boundary
            particles[x][GRID_SIZE - 1].velocity.y = -particles[x][GRID_SIZE - 2].velocity.y; // Reflect at the bottom boundary
        }
    }
    if (property == 1) {
        for (int y = 0; y < GRID_SIZE; y++) {
            particles[0][y].velocity.x = -particles[1][y].velocity.x; // Reflect velocityX at the left boundary
            particles[GRID_SIZE - 1][y].velocity.x = -particles[GRID_SIZE - 2][y].velocity.x; // Reflect at the right boundary
        }
    }
}

// Function to render the grid in black and white
void RenderGrid() {
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int y = 0; y < GRID_SIZE; y++) {
            float value = particles[x][y].density;
            // Clamp the value to a reasonable range (0 to 1)
            value = fmax(0, fmin(1, value / 30.0f)); // Assuming density ranges up to 30
            // Convert density to grayscale (0 = black, 255 = white)
            unsigned char grayValue = (unsigned char)(value * 255);
            Color color = { grayValue, grayValue, grayValue, 255 };
            DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
        }
    }
}

// Function to render the grid in color
//void RenderGrid() {
//    for (int x = 0; x < GRID_SIZE; x++) {
//        for (int y = 0; y < GRID_SIZE; y++) {
//            float value = particles[x][y].density;
//            Color color = { (unsigned char)(value * 255), (unsigned char)(value * 100), (unsigned char)(value * 200), 255 };
//            DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
//        }
//    }
//}

// Main function
int main() {
    // initialize the window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Eulerian Fluid Simulation");
    SetTargetFPS(100); // for my weak machine

    // Initialize particles in the grid
    InitializeParticles();

    // Main simulation loop
    while (!WindowShouldClose()) {
        // Handle mouse interaction
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            Vector2 currentMouse = GetMousePosition();
            float dirX = currentMouse.x - mouse.x; // Calculate direction of mouse movement
            float dirY = currentMouse.y - mouse.y;

            float length = sqrt(dirX * dirX + dirY * dirY);
            if (length > 0) {
                dirX /= length;
                dirY /= length;
            }

            int gridX = currentMouse.x / CELL_SIZE;
            int gridY = currentMouse.y / CELL_SIZE;

            gridX = gridX < 0 ? 0 : (gridX >= GRID_SIZE ? GRID_SIZE - 1 : gridX);
            gridY = gridY < 0 ? 0 : (gridY >= GRID_SIZE ? GRID_SIZE - 1 : gridY);

            // Add velocity and density based on mouse movement
            particles[gridX][gridY].velocity.x += dirX * length * 10;
            particles[gridX][gridY].velocity.y += dirY * length * 10;
            particles[gridX][gridY].density += length * 20;
            mouse = currentMouse;
        }

        // Fluid simulation steps
        Diffuse(0, VISCOSITY);            // Diffuse velocityX. 0 for x component of velocity  
        Diffuse(1, VISCOSITY);            // Diffuse velocityY. 1 for y component of velocity
        Diffuse(2, DIFFUSION_RATE);       // Diffuse density. 2 for density

        // assuming the fluid is incompressible
        Project();                        // Make velocity field incompressible

        Advect(0, DT);                    // Advect velocityX
        Advect(1, DT);                    // Advect velocityY
        Advect(2, DT);                    // Advect density

        Project();                        // Reapply incompressibility after advection

        SetBoundaries(0);                 // Enforce boundary conditions on velocityX like reflection
        SetBoundaries(1);                 // Enforce boundary conditions on velocityY

        // Render the simulation
        BeginDrawing();

        ClearBackground(BLACK);
        
        RenderGrid(); // display the whole fluid

        EndDrawing();
    }

    // Close the window
    CloseWindow();

    return 0;
}