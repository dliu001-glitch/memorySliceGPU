#version 330 core

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;

// TBO containing Screen1 vertex positions
uniform samplerBuffer screen1PositionsTBO;

// Fusion parameters
uniform float mixRatio;
uniform float time;

// Screen2 deformation parameters (match fracture.vert exactly)
uniform float noiseScale;
uniform float noiseStrength;
uniform float breathSpeed;
uniform float breathAmount;
uniform float breathIntensity;
uniform float breathContrast;
uniform float flowFieldStrength;

// All 8 flow field centers
uniform vec3 flowCenter1;
uniform vec3 flowCenter2;
uniform vec3 flowCenter3;
uniform vec3 flowCenter4;
uniform vec3 flowCenter5;
uniform vec3 flowCenter6;
uniform vec3 flowCenter7;
uniform vec3 flowCenter8;

// Fracture parameters
uniform float fractureAmount;
uniform float fractureScale;
uniform float explosionRadius;
uniform float rotationIntensity;
uniform float separationForce;

in vec4 position;
in vec3 normal;

out vec3 worldPosition;
out vec3 worldNormal;
out vec4 debugColor;

// Copy exact noise functions from fracture.vert
float hash3d(vec3 p) {
    return fract(sin(dot(p, vec3(127.1, 311.7, 74.7))) * 43758.5453);
}

float noise3d(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    
    vec3 u = f * f * (3.0 - 2.0 * f);
    
    float n000 = hash3d(i + vec3(0,0,0));
    float n100 = hash3d(i + vec3(1,0,0));
    float n010 = hash3d(i + vec3(0,1,0));
    float n110 = hash3d(i + vec3(1,1,0));
    float n001 = hash3d(i + vec3(0,0,1));
    float n101 = hash3d(i + vec3(1,0,1));
    float n011 = hash3d(i + vec3(0,1,1));
    float n111 = hash3d(i + vec3(1,1,1));
    
    return mix(mix(mix(n000, n100, u.x),
                   mix(n010, n110, u.x), u.y),
               mix(mix(n001, n101, u.x),
                   mix(n011, n111, u.x), u.y), u.z);
}

float noise4d(vec3 p, float t) {
    return noise3d(p + vec3(t * 0.1, t * 0.15, t * 0.12));
}

// Copy rotation matrix function
mat3 rotationMatrix(vec3 axis, float angle) {
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}

// Copy exact fracture calculation from fracture.vert
vec3 calculateFractureOffset(vec3 originalPos, float t) {
    vec3 fractureCenter = vec3(0.0, 0.0, 0.0);
    vec3 toCenter = originalPos - fractureCenter;
    float distanceFromCenter = length(toCenter);
    
    float radialFactor = smoothstep(0.0, explosionRadius, distanceFromCenter);
    float fractureSeed = hash3d(floor(originalPos * fractureScale));
    
    vec3 fractureDirection = normalize(vec3(
        noise3d(originalPos * fractureScale + vec3(100.0, 0.0, 0.0)) - 0.5,
        noise3d(originalPos * fractureScale + vec3(0.0, 100.0, 0.0)) - 0.5,
        noise3d(originalPos * fractureScale + vec3(0.0, 0.0, 100.0)) - 0.5
    ));
    
    vec3 radialForce = normalize(toCenter) * radialFactor;
    vec3 combinedDirection = mix(fractureDirection, radialForce, 0.6);
    
    float timeProgress = smoothstep(0.0, 1.0, t * 0.1);
    float separationDistance = timeProgress * separationForce * radialFactor;
    
    vec3 jitter = vec3(
        noise4d(originalPos * fractureScale * 2.0, t) - 0.5,
        noise4d(originalPos * fractureScale * 2.0 + vec3(50.0), t) - 0.5,
        noise4d(originalPos * fractureScale * 2.0 + vec3(100.0), t) - 0.5
    ) * 5.0;
    
    return combinedDirection * separationDistance + jitter;
}

// Copy exact rotation logic
vec3 applyFractureRotation(vec3 pos, vec3 originalPos, float t) {
    vec3 rotationAxis = normalize(vec3(
        noise3d(originalPos * fractureScale + vec3(200.0)),
        noise3d(originalPos * fractureScale + vec3(300.0)),
        noise3d(originalPos * fractureScale + vec3(400.0))
    ));
    
    float rotationSpeed = hash3d(originalPos * fractureScale) * 2.0 + 1.0;
    float rotationAngle = t * rotationSpeed * rotationIntensity;
    
    vec3 fractureCenter = originalPos;
    vec3 relativePos = pos - fractureCenter;
    
    mat3 rotation = rotationMatrix(rotationAxis, rotationAngle);
    vec3 rotatedPos = rotation * relativePos;
    
    return rotatedPos + fractureCenter;
}

// Copy exact flow field calculation
vec3 calculateFlowField(vec3 pos, float t) {
    float dynamicScale = mix(0.005, 0.002, clamp(flowFieldStrength / 500.0, 0.0, 1.0));
    float flowTime = t * 0.15;
    
    float n1 = noise4d(vec3(pos.y * dynamicScale, pos.z * dynamicScale, 0.0), flowTime);
    float n2 = noise4d(vec3(pos.z * dynamicScale, pos.x * dynamicScale, 100.0), flowTime);
    float n3 = noise4d(vec3(pos.x * dynamicScale, pos.y * dynamicScale, 200.0), flowTime);
    
    vec3 flow;
    float smoothFactor = mix(2.0, 4.0, clamp(flowFieldStrength / 200.0, 0.0, 1.0));
    flow.x = sin(n1 * 6.28318) * cos(pos.y * 0.002) * smoothFactor;
    flow.y = sin(n2 * 6.28318) * cos(pos.z * 0.002) * smoothFactor;
    flow.z = sin(n3 * 6.28318) * cos(pos.x * 0.002) * smoothFactor;
    
    vec3 flowCenters[8];
    flowCenters[0] = flowCenter1;
    flowCenters[1] = flowCenter2; 
    flowCenters[2] = flowCenter3;
    flowCenters[3] = flowCenter4;
    flowCenters[4] = flowCenter5;
    flowCenters[5] = flowCenter6;
    flowCenters[6] = flowCenter7;
    flowCenters[7] = flowCenter8;
    
    float baseInfluenceRadius = length(pos) * 0.8;
    float influenceRadius = mix(baseInfluenceRadius, baseInfluenceRadius * 2.0, clamp(flowFieldStrength / 300.0, 0.0, 1.0));
    
    for (int i = 0; i < 8; i++) {
        vec3 center = flowCenters[i];
        float dist = length(pos - center);
        
        if (dist < influenceRadius) {
            float strength = 1.0 - (dist / influenceRadius);
            strength = smoothstep(0.0, 1.0, strength);
            
            vec3 toCenter = center - pos;
            vec3 tangent = cross(toCenter, vec3(0, 1, 0));
            if (length(tangent) > 0.001) {
                tangent = normalize(tangent);
            }
            
            float spiralIntensity = mix(1.5, 3.0, clamp(flowFieldStrength / 250.0, 0.0, 1.0));
            vec3 spiral = tangent * strength * spiralIntensity + vec3(0, strength * spiralIntensity * 1.5, 0);
            flow += spiral * 1.5;
        }
    }
    
    // Global flow effects
    float globalAmplitude = mix(0.5, 1.5, clamp(flowFieldStrength / 200.0, 0.0, 1.0));
    flow += vec3(
        sin(t * 0.3 + pos.x * 0.006) * globalAmplitude,
        cos(t * 0.2) * globalAmplitude * 1.2,
        sin(t * 0.25 + pos.z * 0.006) * globalAmplitude
    );
    
    if (flowFieldStrength > 150.0) {
        float extraStrength = (flowFieldStrength - 150.0) / 150.0;
        vec3 expansionForce = normalize(pos) * extraStrength * 2.0;
        
        vec3 randomDirection = vec3(
            noise4d(pos * 0.01, t * 0.1) - 0.5,
            noise4d(pos * 0.01 + vec3(50.0), t * 0.1) - 0.5,
            noise4d(pos * 0.01 + vec3(100.0), t * 0.1) - 0.5
        );
        randomDirection = normalize(randomDirection) * extraStrength * 1.5;
        
        flow += expansionForce + randomDirection;
    }
    
    return flow;
}

void main() {
    vec3 originalPos = position.xyz;
    vec3 newPos = originalPos;
    
    // Apply exact Screen2 deformations
    float timeOffset = time * 0.3;
    
    // 1. Noise displacement
    float noiseX = noise4d(originalPos * noiseScale, timeOffset) * 2.0 - 1.0;
    float noiseY = noise4d(originalPos * noiseScale + vec3(100.0), timeOffset) * 2.0 - 1.0;
    float noiseZ = noise4d(originalPos * noiseScale + vec3(200.0), timeOffset) * 2.0 - 1.0;
    
    newPos += vec3(noiseX, noiseY, noiseZ) * noiseStrength;
    
    // 2. Enhanced breathing effect (exact copy from fracture.vert)
    float baseBreathPhase = sin(time * breathSpeed) * 0.5 + 0.5;
    float enhancedPhase = pow(baseBreathPhase, 1.0 / breathContrast);
    float primaryBreath = enhancedPhase;
    float secondaryBreath = sin(time * breathSpeed * 0.5) * 0.3 + 0.7;
    float finalBreathPhase = mix(secondaryBreath, primaryBreath, 0.7);
    
    vec3 breathOffset = normalize(originalPos) * breathAmount * finalBreathPhase * breathIntensity;
    newPos += breathOffset;
    
    // 3. Flow field
    vec3 flowForce = calculateFlowField(originalPos, time);
    newPos += flowForce * flowFieldStrength;
    
    // 4. Fracture effects
    if (fractureAmount > 0.0) {
        vec3 fractureOffset = calculateFractureOffset(originalPos, time);
        newPos += fractureOffset * fractureAmount;
        newPos = applyFractureRotation(newPos, originalPos, time);
    }
    
    // 5. Apply offset limits (copy from fracture.vert)
    vec3 offset = newPos - originalPos;
    float baseMaxOffset = 100.0;
    float fractureBonus = fractureAmount * 200.0;
    float flowFieldBonus = clamp(flowFieldStrength * 0.8, 0.0, 400.0);
    float breathBonus = clamp(breathAmount * breathIntensity * 0.5, 0.0, 100.0);
    
    float dynamicMaxOffset = baseMaxOffset + fractureBonus + flowFieldBonus + breathBonus;
    
    float offsetLength = length(offset);
    if (offsetLength > dynamicMaxOffset) {
        float limitFactor = dynamicMaxOffset / offsetLength;
        limitFactor = smoothstep(0.8, 1.0, limitFactor);
        offset *= limitFactor;
        newPos = originalPos + offset;
    }
    
    vec3 screen2Pos = newPos; // This is now the properly deformed Screen2 position
    
    // Sample Screen1 position from TBO
    vec3 screen1Pos = vec3(0.0);
    int tboSize = textureSize(screen1PositionsTBO);
    
    if (tboSize > 0) {
        int screen1Index = gl_VertexID % tboSize;
        screen1Pos = texelFetch(screen1PositionsTBO, screen1Index).xyz;
    }
    
    // Spatial fusion
    vec3 fusedPosition = mix(screen2Pos, screen1Pos, mixRatio);
    
    vec4 worldPos = modelViewMatrix * vec4(fusedPosition, 1.0);
    worldPosition = worldPos.xyz;
    worldNormal = normalize((modelViewMatrix * vec4(normal, 0.0)).xyz);
    
    debugColor = vec4(1.0 - mixRatio, mixRatio, 0.5, 1.0);
    
    gl_Position = modelViewProjectionMatrix * vec4(fusedPosition, 1.0);
}