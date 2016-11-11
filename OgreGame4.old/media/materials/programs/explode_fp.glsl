uniform sampler3D tex;
 
void main()
{
   vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
   vec3 textureAccessIndex = vec3(0.0, 0.0, 0.0);
   textureAccessIndex.x = gl_TexCoord[0].s;
   textureAccessIndex.y = gl_TexCoord[0].t;
   textureAccessIndex.z = gl_Color.a * 16.0;
   color = texture3D(tex, textureAccessIndex);
   gl_FragColor = color;
}