import { Canvas, useThree } from '@react-three/fiber'
import { OrbitControls, Environment, ContactShadows, Grid } from '@react-three/drei'
import { Suspense, useState, useEffect } from 'react'
import Model from './Model'
import './ModelViewer.css'

function LoadingFallback() {
  return (
    <mesh>
      <boxGeometry args={[1, 1, 1]} />
      <meshStandardMaterial color="orange" wireframe />
    </mesh>
  )
}

function ModelViewer() {
  const [error, setError] = useState(null)
  const [loading, setLoading] = useState(true)

  useEffect(() => {
    console.log('ModelViewer mounted')
    const timer = setTimeout(() => setLoading(false), 3000)
    return () => clearTimeout(timer)
  }, [])

  return (
    <div className="viewer-canvas">
      {error && (
        <div className="error-message">
          Error loading model: {error}
        </div>
      )}
      {loading && <div className="loading-message">Loading 3D model...</div>}
      <Canvas
        shadows
        camera={{ position: [3, 3, 3], fov: 60 }}
        resize={{ scroll: false, debounce: 0 }}
        onCreated={({ gl }) => {
          console.log('Canvas created')
          gl.setClearColor('#1a1a1a')
        }}
        onError={(e) => {
          console.error('Canvas error:', e)
          setError(e.message)
        }}
      >
        {/* Lighting setup */}
        <ambientLight intensity={0.6} />
        <directionalLight
          position={[5, 5, 5]}
          intensity={1}
          castShadow
        />
        <pointLight position={[-5, 3, -5]} intensity={0.3} />
        
        {/* Environment for reflections */}
        <Environment preset="city" />
        
        {/* 3D Model */}
        <Suspense fallback={<LoadingFallback />}>
          <Model />
        </Suspense>
        
        {/* Ground shadow */}
        <ContactShadows
          position={[0, -2, 0]}
          opacity={0.3}
          scale={20}
          blur={2}
          far={10}
        />
        
        {/* Camera Controls */}
        <OrbitControls
          enablePan={true}
          enableZoom={true}
          enableRotate={true}
          minDistance={0.5}
          maxDistance={50}
        />
      </Canvas>
    </div>
  )
}

export default ModelViewer

