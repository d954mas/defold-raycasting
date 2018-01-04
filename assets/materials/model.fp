varying mediump vec2 var_texcoord0;

uniform mediump sampler2D tex0;
uniform mediump vec4 led;

void main(){
	gl_FragColor = texture2D(tex0, var_texcoord0.xy);
}
