#extension GL_OES_standard_derivatives : enable

precision highp float;

uniform vec3 color;

varying vec3 fPosition;
varying vec3 fNormal;

void main()
{
  vec3 normal = normalize(fNormal);
  vec3 eye = normalize(-fPosition.xyz);
  float rim = smoothstep(0.5, 0.8, 1.0 - dot(normal, eye));
  gl_FragColor = vec4( clamp(rim, 0.0, 1.0) * 1. * color, 1.0 );
}
