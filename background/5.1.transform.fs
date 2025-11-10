#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 ourColor;      // El color con transparencia
uniform sampler2D texture1; // La textura
uniform bool useTexture;    // Controla si se debe usar la textura o no

void main()
{
    if (useTexture)
    {
        // Si la textura está activa, mezclar con el color
        vec4 texColor = texture(texture1, TexCoord);
        FragColor = mix(texColor, ourColor, ourColor.a); // Mezcla entre textura y color según la transparencia
    }
    else
    {
        // Si la textura está desactivada, solo usar el color con transparencia
        FragColor = ourColor;
    }
}

