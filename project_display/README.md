# SpotifyGesture - Project Website

Interactive project showcase website for SpotifyGesture, featuring 3D model visualization, comprehensive documentation, engineering process breakdown, and team information.

## Overview

This website is built with **Vite + React** and showcases the complete SpotifyGesture project. It includes:

- **Interactive 3D Model Viewer**: Rendered with Three.js and React Three Fiber, displaying the custom-built apparatus
- **Project Overview**: Complete project description, hardware list, specifications, and achievements
- **Engineering Process**: Step-by-step breakdown of the development process from concept to integration
- **Team Profiles**: Detailed information about each team member and their contributions
- **Demo Videos**: POV and third-person demonstrations of the system in action
- **Source Code**: Links and documentation for the GitHub repository

## Tech Stack

- **React 18** - UI framework
- **Vite** - Build tool and dev server
- **Three.js** - 3D rendering engine
- **React Three Fiber** - React renderer for Three.js
- **React Three Drei** - Useful helpers for React Three Fiber
- **STLLoader** - For loading and rendering STL files

## Setup

### Prerequisites

- Node.js (v16 or higher)
- npm or yarn

### Installation

1. **Install dependencies:**
   ```bash
   npm install
   ```

2. **Ensure assets are in place:**
   - The STL file should be at `public/Spotify-Gesture.stl`
   - All images should be in `public/` subdirectories
   - Demo videos should be in `public/`

## Development

Run the development server:
```bash
npm run dev
```

The site will be available at `http://localhost:5173`

### Development Notes

- The Vite config is set up to handle different base paths for development vs. production
- In development, base path is `/`
- In production (GitHub Pages), base path is `/SpotifyGesture/`

## Building for Production

Build the site:
```bash
npm run build
```

Preview the production build locally:
```bash
npm run preview
```

The build output will be in the `dist/` directory.

## Deploying to GitHub Pages

The repository includes an automatic deployment workflow (`.github/workflows/deploy.yml`).

### Automatic Deployment

1. Push changes to the `main` branch
2. GitHub Actions will automatically build and deploy to GitHub Pages
3. The site will be live at `https://newmie10.github.io/SpotifyGesture/`

### Manual Setup (First Time)

1. Enable GitHub Pages in repository settings
2. Set the source to the `gh-pages` branch
3. The deployment workflow will handle the rest

## Project Structure

```
project_display/
├── public/                    # Static assets
│   ├── Spotify-Gesture.stl   # 3D model file
│   ├── HardwareDesign/       # Hardware images
│   ├── HandDetector/         # ML model images
│   ├── *.png, *.jpg          # Team photos and other images
│   └── *.mp4                 # Demo videos
├── src/
│   ├── components/
│   │   ├── ModelViewer.jsx   # 3D canvas wrapper
│   │   ├── Model.jsx         # STL model loader
│   │   └── ModelViewer.css   # Viewer styles
│   ├── App.jsx               # Main application component
│   ├── App.css               # Main styles
│   ├── index.css             # Global styles
│   └── main.jsx              # React entry point
├── .github/workflows/
│   └── deploy.yml            # GitHub Pages deployment
├── index.html                # HTML entry point
├── vite.config.js            # Vite configuration
├── package.json              # Dependencies and scripts
└── README.md                 # This file
```

## Features

### 3D Model Viewer
- Smooth orbital controls for viewing from any angle
- Professional lighting setup (ambient + directional)
- Spotify green material (#1DB954)
- Responsive canvas that adapts to tab changes
- Optimized performance with proper geometry handling

### Responsive Layout
- Dynamic viewer sizing based on active tab
- Full-screen 3D model on the Model tab
- 25% width viewer on other tabs with smooth transitions
- Fully scrollable content sections

### Tab Navigation
- Project Overview with specs and achievements
- Engineering Process with interactive flowchart
- Team member profiles with photos and links
- Demo videos with descriptions
- Source code information and repository structure

## Customization

### Updating the 3D Model

If you need to replace the 3D model:
1. Place your new STL file in `public/`
2. Update the path in `src/components/Model.jsx` (line with `useLoader(STLLoader, ...)`)
3. Adjust rotation, position, and scale in `Model.jsx` as needed

**Note:** The current model positioning is carefully calibrated. See the memory note in `Model.jsx` before making changes.

### Styling

- Main color theme is Spotify green (#1DB954)
- All styles are in `src/App.css` and `src/index.css`
- Component-specific styles in `src/components/*.css`

## Course Information

**Course:** CICS 256 - Make: A Hands-on Introduction to Physical Computing  
**Institution:** University of Massachusetts Amherst  
**Semester:** Fall 2025  
**Instructor:** Professor Md Farhan Tanism

## Team

- **Austin Fairbanks** - Hand detection ML model, camera integration, apparatus design
- **Ian Rapko** - Hardware integration, screen testing, code migration, apparatus construction
- **Sam Newman** - Gesture detection, ToF sensors, Spotify API integration, serial communication

## License

© 2025 SpotifyGesture Team. Built for CICS 256 at UMass Amherst.

## Links

- **Live Website**: [https://newmie10.github.io/SpotifyGesture/](https://newmie10.github.io/SpotifyGesture/)
- **GitHub Repository**: [https://github.com/newmie10/SpotifyGesture](https://github.com/newmie10/SpotifyGesture)
- **Parent Project**: See repository root for hardware/firmware code
