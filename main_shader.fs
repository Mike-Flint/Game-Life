#version 330 core

uniform sampler2D gridTexture;
uniform sampler2D gridGameTexture;
uniform vec2 resolution;
uniform int cellSize;

out vec4 FragColor;

void main() {
    float cols = resolution.x / cellSize;
    float rows = resolution.y / cellSize;

    vec2 uv;

    uv.x =  (gl_FragCoord.x / resolution.x);
    uv.y = 1.0 - (gl_FragCoord.y / resolution.y);

    vec3 cellState = texture(gridTexture, uv).rgb;
    vec3 pointCellState = texture(gridGameTexture, uv).rgb;


    vec2 cellUV = fract(uv * vec2(cols, rows));
    if (cellUV.x < 0.10 || cellUV.y < 0.10) {
        FragColor = vec4(0.2, 0.2, 0.2, 1.0);
    }
    else if (pointCellState.r == 1.0) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    } 
    else if (cellState.r == 1.0) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    } 
    else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }

}
