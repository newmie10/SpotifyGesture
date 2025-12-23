import { useLoader } from '@react-three/fiber'
import { STLLoader } from 'three/examples/jsm/loaders/STLLoader'
import { Center } from '@react-three/drei'
import { useEffect, useMemo } from 'react'
import * as THREE from 'three'

function Model() {
  // Load the STL file from the public directory
  const geometry = useLoader(STLLoader, '/Spotify-Gesture.stl')
  
  useEffect(() => {
    // Compute vertex normals for better lighting
    geometry.computeVertexNormals()
    geometry.center()
    
    // Log bounding box to see the size
    geometry.computeBoundingBox()
    const bbox = geometry.boundingBox
    console.log('Model loaded successfully!')
    console.log('Bounding Box:', bbox)
    console.log('Size:', {
      x: bbox.max.x - bbox.min.x,
      y: bbox.max.y - bbox.min.y,
      z: bbox.max.z - bbox.min.z
    })
  }, [geometry])
  
  return (
    <Center>
      <mesh 
        geometry={geometry} 
        castShadow 
        receiveShadow 
        scale={0.01}
        rotation={[-Math.PI / 2 - Math.PI * 3.5 / 180, 0, -Math.PI * 95 / 180 + Math.PI / 2 - Math.PI * 3 / 180]}
        position={[1.41, 1.4, -0.75]}
      >
        <meshStandardMaterial 
          color="#1DB954" 
          metalness={0.5} 
          roughness={0.3} 
          side={THREE.DoubleSide}
        />
      </mesh>
    </Center>
  )
}

export default Model

