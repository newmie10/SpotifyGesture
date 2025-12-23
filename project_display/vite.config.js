import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vitejs.dev/config/
export default defineConfig(({ command }) => {
  return {
    plugins: [react()],
    // Use '/' for development, '/SpotifyGesture/' for production
    base: command === 'serve' ? '/' : '/SpotifyGesture/',
    build: {
      outDir: 'dist',
    },
    assetsInclude: ['**/*.stl', '**/*.obj', '**/*.gltf', '**/*.glb'],
    server: {
      headers: {
        'Cross-Origin-Embedder-Policy': 'credentialless',
        'Cross-Origin-Opener-Policy': 'same-origin',
      },
    },
  }
})
