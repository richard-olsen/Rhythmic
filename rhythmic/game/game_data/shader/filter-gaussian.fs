#version 150

uniform sampler2D textureSample;

uniform vec2 direction;

in vec2 tex_coord;

out vec4 fragColor;

void main()
{
    vec3 sum = vec3(0);
    //sum += (texture(textureSample, tex_coord - (direction * 3)) * 0.00598);
    //sum += (texture(textureSample, tex_coord - (direction * 2)) * 0.060626);
    //sum += (texture(textureSample, tex_coord - (direction)) * 0.241843);
    //sum += (texture(textureSample, tex_coord) * 0.383103);
    //sum += (texture(textureSample, tex_coord + (direction)) * 0.241843);
    //sum += (texture(textureSample, tex_coord + (direction * 2)) * 0.060626);
    //sum += (texture(textureSample, tex_coord + (direction * 3)) * 0.00598);
    sum += (texture(textureSample, tex_coord - (direction * 4)).xyz * 0.000229);
    sum += (texture(textureSample, tex_coord - (direction * 3)).xyz * 0.005977);
    sum += (texture(textureSample, tex_coord - (direction * 2)).xyz * 0.060598);
    sum += (texture(textureSample, tex_coord - (direction)).xyz * 0.241732);
    sum += (texture(textureSample, tex_coord).xyz * 0.382928);
    sum += (texture(textureSample, tex_coord + (direction)).xyz * 0.241732);
    sum += (texture(textureSample, tex_coord + (direction * 2)).xyz * 0.060598);
    sum += (texture(textureSample, tex_coord + (direction * 3)).xyz * 0.005977);
    sum += (texture(textureSample, tex_coord + (direction * 4)).xyz * 0.000229);
    fragColor = vec4(sum, 1);
}
