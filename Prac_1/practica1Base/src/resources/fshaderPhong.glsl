#if __VERSION__<130
#define IN varying
#define OUT varying
#else
#define IN in
#define OUT out
#endif

IN vec4 Position;
IN vec3 Normal;
IN vec2 v_texcoord;


/*
 * Structura para pasar los matereiales
 */
struct Intensity
{
    vec3 kd;
    vec3 ks;
    vec3 ka;
    float shininess;
};
uniform Intensity IMaterial;

/*
 * Structura para pasar las luces
 */
struct Light
{
    vec3 ambiental;
    vec3 especular;
    vec3 difusa;
    vec4 coordenadas;
    vec4 direccion;
    int tipo;
    float angulo;
    float a;
    float b;
    float c;
    bool on;
};
uniform Light luz[3];

/*
 * Uniform para la luz ambiente (Global)
 */
uniform vec3 vLuzAmbiente;

/*
 * Uniform para la imagen
 */
uniform sampler2D texMap;

/*
 * uniform para activar o desactivar el PhongTextures
 */
uniform bool vPhongTex;


/*
 * Función para calcular la atenuación de una luz
 */
float calculateAtenuation(int i){

    float a = luz[i].a;
    float b = luz[i].b;
    float c = luz[i].c;
    float d = length(luz[i].coordenadas.xyz - Position.xyz); // Distancia de la luz respecto al objeto
    float powD = pow(d, 2.0);
    float dividendo = (a * powD) + (b * d) + c;

    return 1.0f/dividendo;
}

/*
 * Función para calcular la parte de phong para cada luz
 */
vec3 calculatePhong(int i, vec3 L){
    vec3 vObs = vec3(0.0,0.0,10.0);
    vec3 V = normalize(vObs - Position.xyz);  // Posicion Camara
    vec3 N = Normal;            // Normal de vertice
    vec3 H = normalize(L+V);    // Vector medio normalizado

    vec3 d = (luz[i].difusa * IMaterial.kd) * max(dot(L,N),0.0);
    vec3 s = (luz[i].especular * IMaterial.ks) * pow(max(dot(N,H),0.0), IMaterial.shininess);
    vec3 a = (luz[i].ambiental * IMaterial.ka);


    return d+s+a;
}


/*
 * Calcula si la posición esta dentro del angulo de la luz
 */
bool colorSpotLaight(int i){

    vec3 direcc = normalize(luz[i].coordenadas.xyz - Position.xyz);

    float angulo = dot(luz[i].direccion.xyz, direcc);

    if (angulo > luz[i].angulo)
        return true;

    return false;
}


/*
 * Devuelve la texura si hay
 */
vec4 returnTextures(vec4 ITotal){
    return (vPhongTex == true ? (0.25*ITotal + 0.75*texture2D(texMap, v_texcoord)): ITotal);
}

void main()
{

    vec3 phong1 = vec3(0.0, 0.0, 0.0);
    vec3 phong2 = vec3(0.0, 0.0, 0.0);
    vec3 phong3 = vec3(0.0, 0.0, 0.0);

    // Luz Puntual
    if (luz[0].on == true){
        vec3 L = normalize(luz[0].coordenadas.xyz - Position.xyz); // de la luz respecto al objeto
        phong1 = calculateAtenuation(0) * calculatePhong(0, L);
    }

    // Luz Direccional
    if (luz[1].on == true){
        vec3 L = normalize(luz[1].direccion.xyz - Position.xyz); // de la luz respecto al objeto
        phong2 = 1.0f * calculatePhong(1, L);
    }

    // Luz SpotLight
    if (luz[2].on == true){
//        vec3 L = normalize(luz[2].direccion.xyz - Position.xyz); // de la luz respecto al objeto
        vec3 L = normalize(luz[2].coordenadas.xyz - Position.xyz);
        /* Se calcula con la dirección + anchura */

        if (colorSpotLaight(2))
            phong3 = calculateAtenuation(2) * calculatePhong(2, L);
        else
            phong3 = vLuzAmbiente * IMaterial.ka;
    }

    vec4 ITotal = vec4((vLuzAmbiente * IMaterial.ka) + (phong1 + phong2 + phong3),1.0);

//    if (ITotal.x > 1.0){
//        ITotal.x = 1.0;
//    }
//    if (ITotal.y > 1.0){
//        ITotal.y = 1.0;
//    }
//    if (ITotal.z > 1.0){
//        ITotal.z = 1.0;
//    }

    gl_FragColor = returnTextures(ITotal);
}
