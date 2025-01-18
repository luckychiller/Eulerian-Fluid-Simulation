# Eulerian Fluid Simulation

This project is an implementation of a 2D fluid simulation based on the Eulerian approach, using the Raylib library for visualization. It simulates the diffusion, advection, and incompressibility of fluid density and velocity fields on a grid. Users can interact with the simulation through mouse inputs to observe changes in the fluid's behavior.

## Features

- **Fluid Simulation:** Implements key fluid dynamics principles, including diffusion, advection, and projection for incompressibility.
- **Visualization:** Renders the fluid density on a grid in black and white.
- **Mouse Interaction:** Allows users to interact with the fluid by adding velocity and density through mouse movements.
- **Customizable Parameters:** Parameters such as grid size, diffusion rate, viscosity, and time step can be easily adjusted.

## Requirements

- **Compiler:** C compiler (e.g., GCC or Clang).
- **Raylib:** Install the Raylib library for rendering.
- **Dependencies:** Math library (`math.h`) for calculations.

## Setup Instructions

1. **Install Raylib:**
   - Follow the installation guide from the official [Raylib website](https://www.raylib.com/).

2. **Clone the Repository:**
   ```bash
   git clone <repository_url>
   cd <repository_folder>
   ```

3. **Build the Project:**
   Compile the code using your preferred C compiler. For example:
   ```bash
   gcc -o fluid_simulation fluid_simulation.c -lraylib -lm
   ```

4. **Run the Simulation:**
   ```bash
   ./fluid_simulation
   ```

## Controls

- **Mouse Left Button:**
  - Click and drag to add velocity and density to the fluid.
- **Close Button:**
  - Close the window to exit the simulation.

## Code Overview

### Key Components

- **Grid Representation:**
  - `Eulerian_Particle`: Represents each cell in the grid with properties like position, velocity, density, and color.
  - `particles[GRID_SIZE][GRID_SIZE]`: 2D array of particles.

- **Simulation Functions:**
  - `InitializeParticles()`: Initializes the grid with default values.
  - `Diffuse()`: Simulates the diffusion of properties (density and velocity).
  - `Advect()`: Moves properties across the grid based on velocity.
  - `Project()`: Ensures the velocity field remains incompressible.
  - `SetBoundaries()`: Enforces boundary conditions on the grid.

- **Rendering:**
  - `RenderGrid()`: Visualizes the grid as a black-and-white representation of density.

### Parameters

| Parameter          | Description                              | Default Value |
|--------------------|------------------------------------------|---------------|
| `GRID_SIZE`        | Number of cells along one dimension.     | 150           |
| `SCREEN_WIDTH`     | Width of the simulation window.          | 600           |
| `SCREEN_HEIGHT`    | Height of the simulation window.         | 600           |
| `DIFFUSION_RATE`   | Rate at which density diffuses.          | 0.1f          |
| `VISCOSITY`        | Fluid viscosity.                         | 0.0001f       |
| `DT`               | Time step for the simulation.            | 0.1f          |

## Customization

- **Grid Size:**
  Adjust `GRID_SIZE` to increase or decrease the resolution of the simulation.
- **Visualization Mode:**
  Uncomment the alternate `RenderGrid()` function to visualize in color.
- **Interaction Strength:**
  Modify the velocity and density increments in the mouse interaction section to change the effect of user input.

## Example Output

The simulation displays a grid where the density field evolves over time. Interacting with the simulation adds swirls and movements, creating a dynamic fluid-like visualization.

### Sample GIFs

Below are examples of the fluid simulation in action:
- **black_and_white:**
  ![black_and_white](black_and_white.gif)

- **Colored**
  ![Colored](Colored.gif)


## License

This project is licensed under the MIT License. Feel free to use, modify, and distribute the code.

## Acknowledgments

- **Raylib:** For providing a simple and efficient library for 2D and 3D game programming.
- **Stam's Stable Fluids:** Inspiration for the underlying simulation techniques.

Enjoy simulating fluid dynamics!

