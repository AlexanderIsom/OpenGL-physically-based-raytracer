#version 430 core
out vec4 fColor;

in vec3 vertexPos;

uniform mat4 inverseViewMatrix;
uniform mat4 inverseProjectionMatrix;

int id;

#define PI 3.1415926535897932384626433832795


struct Ray{
	vec3 origin;
	vec3 direction;
};

struct intersectResult{
	vec3 pos;
	float dist;
	bool hit;
	vec4 color;
	float shinyness;
	float radius;
};

vec4 backGroundColor = vec4(0.0, 0.0 ,0.0,1.0);

Ray genRay(vec3 vertexPos);
Ray reflectionRay(Ray ray, intersectResult result);
intersectResult intersect(Ray ray);
intersectResult simpleIntersect(Ray ray);
vec4 shade(intersectResult result, Ray ray);

struct Sphere
{
	vec3 pos;
	vec4 color;
	float radius;
	float shinyness;
	
};

Sphere objects[2];

struct LightSrc{
	vec3 pos;
	vec3 direction;
	vec4 color;
	float strength;
};

LightSrc light;

float facing(vec3 a, vec3 b){
	if(dot(a,b) > 0.0f)
	{
		return 1.0f;
	}
	return 0.0f;
};

Ray genRay(vec3 vertexPos){

	Ray ray;

	vec4 startPoint = vec4(vertexPos.x,vertexPos.y,-1,1);
	vec4 endPoint = vec4(vertexPos.x,vertexPos.y,1,1);

	startPoint = inverseProjectionMatrix * startPoint;
	startPoint = inverseViewMatrix * startPoint;
	startPoint = startPoint / startPoint.w;
	
	endPoint = inverseProjectionMatrix * endPoint;
	endPoint = inverseViewMatrix * endPoint;
	endPoint = endPoint / endPoint.w;

	ray.origin = vec3(startPoint);
	ray.direction = vec3(normalize(endPoint - startPoint));

	return ray;
}

Ray reflectionRay(Ray ray, intersectResult result){

	//intersection point
	vec3 intersect = ray.origin + (result.dist)*normalize(ray.direction);
	vec3 surfaceNormal = normalize(intersect - result.pos);

	vec3 newRayDir = (ray.direction)+2*(dot(-ray.direction,surfaceNormal))*surfaceNormal;

	ray.direction = newRayDir;
	ray.origin = (intersect + (newRayDir*0.0001f));

	return ray;
}

intersectResult simpleIntersect(Ray ray){
	intersectResult rtn;
	rtn.dist = -1;
	rtn.hit = false;
	rtn.color = backGroundColor;

	vec3 n = normalize(ray.direction);

	for(int i = 0; i < objects.length(); i++)
	{
		vec3 pa = objects[i].pos - ray.origin;
		float dist = length(pa);
		float a = dot(pa,n);

		//its inside the sphere, draw blue sphere
		if(dist <= objects[i].radius)
		{
			rtn.color = vec4(0.0, 0.0,1.0,1.0);		   
			//goto next object
			rtn.hit = false;
			continue;
		}

		vec3 dVec = pa - (a*n);
		dist = length(dVec);

		if(dist <= objects[i].radius)
		{
			float x = sqrt(pow(objects[i].radius,2)-pow(dist,2));
			float hitDist = a-x;
			if(hitDist < rtn.dist || rtn.dist < 0 && hitDist != rtn.dist && hitDist > 0)
			{		
				rtn.hit = true;
			}
		}
	}

return rtn;
}


intersectResult intersect(Ray ray)
{
	intersectResult rtn;
	rtn.dist = -1;
	rtn.hit = false;
	rtn.color = backGroundColor;

	vec3 n = normalize(ray.direction);

	for(int i = 0; i < objects.length(); i++)
	{
		vec3 pa = objects[i].pos - ray.origin;
		float dist = length(pa);
		float a = dot(pa,n);

		//its inside the sphere, draw blue sphere
		if(dist <= objects[i].radius)
		{
			rtn.color = vec4(0.0, 0.0,1.0,1.0);		   
			//goto next object
			rtn.hit = false;
			continue;
		}	

		//object is behind camera, draw purple sphere
		if(dot(pa,n) < 0)
		{	
			rtn.color = vec4(1.0, 0.0,1.0,1.0);
			//go to next object
			rtn.hit = false;
			continue;
		}


		//d = (p-a) - ((dot(a,n)) * n)
		vec3 dVec = pa - (a*n);
		dist = length(dVec);

		if(dist <= objects[i].radius)
		{
			float x = sqrt(pow(objects[i].radius,2)-pow(dist,2));
			float hitDist = a-x;
			if(hitDist < rtn.dist || rtn.dist < 0 && hitDist != rtn.dist && hitDist > 0)
			{		
				rtn.hit = true;
				rtn.dist = hitDist;
				rtn.color = objects[i].color;
				rtn.pos = objects[i].pos;
				rtn.shinyness = objects[i].shinyness;
				rtn.radius = objects[i].radius;
			}
		}
	}

return rtn;
}

float DistributionGGX(vec3 N, vec3 H, float a)//n = surface normal, h halfway vector between surface normal and light direction, a surface roughness
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float k)//n = surface nornmal, v = view direction, k = 
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) //cos theta = dot of surface normal and view direciton
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 diffuseLambert(vec3 albedo) // albedo of pixel
{
	return albedo / PI;
}

vec4 shade(intersectResult result, Ray ray){
		
		vec3 n = normalize(ray.direction);


		//this needs replacing with brdf!
		//light = BRDF*IL*dot(N,L)
		//I is intensity
		//BRDF defines material reflectance properties
		//IL is light intensity
		//Dot(N,L) is dot product between normal and light direction		

		vec4 diffuseBRDF ; 
		vec4 specularBRDF ; 



		//intersection point
		vec3 intersect = ray.origin + (result.dist)*n;

		//get surface normal
		vec3 surfaceNormal = normalize(intersect - result.pos);

		//light direction
		light.direction = normalize(light.pos - intersect);
		//view direction
		vec3 viewDir = normalize(vertexPos - intersect);
		
		vec3 lightDir = normalize(surfaceNormal - light.direction);



		//BRDF shading
		vec3 f0 = vec3(0.04);
		f0 = mix(f0, vec3(result.color), result.shinyness);


		float cosTheta = dot(surfaceNormal, viewDir);

		diffuseBRDF = vec4((1-fresnelSchlick(cosTheta, f0))*diffuseLambert(vec3(1.0f)), 1.0f);

		specularBRDF = vec4((fresnelSchlick(cosTheta, f0)*DistributionGGX(surfaceNormal, lightDir, 1.0f)*GeometrySchlickGGX(cosTheta, 1.0f)/4*(dot(surfaceNormal,light.direction)*dot(surfaceNormal,viewDir))),1.0f);

		
		
		
		//SHADING Phong
		vec3 midDir = normalize(light.direction + viewDir);


		//phong shading model // needs to be updated with pbr
			
		float ambientStr = 10.1f;

		vec4 ambient = vec4(ambientStr);
		vec4 diffuse;
		vec4 specular;


		//make a ray to the light and test for intersections, if intersection happens then its in shadow
		Ray lightRay;
		intersectResult shadowResult;
		lightRay.direction = normalize(light.pos - intersect);
		lightRay.origin = intersect + (lightRay.direction * 0.0001f);

		shadowResult = simpleIntersect(lightRay);


		//if intersection 
		if(shadowResult.hit)
		{			
			//make diffuse and specular 0
			diffuse = vec4(0.0f);
			specular = vec4(0.0f);
		}else{
			//else shade appropriately 
			diffuse = vec4(max(dot(surfaceNormal, light.direction),0.0f));
			specular = vec4(facing(surfaceNormal, light.direction) * pow(max(dot(surfaceNormal, midDir), 0.0f), result.shinyness));
		}

		vec4 outColor = ambient + diffuse + specular; // reflectivity


		//get coordinates of texture using spherical coordinate system
		//text color replaces surface color as instead of a color it will get it from a texture
		//might be good to test a bool here depending on the result of the intersection, if it is a textured object or just a colored object
		vec3 intPos = result.pos - intersect;
		float theta = atan(intPos.x/intPos.z);
		float a = sqrt(pow(intPos.x,2)+pow(intPos.z,2));
		float sTheta = asin(a/result.radius);

		float sphereX = theta / (2*PI);
		float sphereY = sTheta / (2*PI);

		vec4 textColor = vec4(sphereX,sphereY,0,1.0f);

		outColor = outColor * light.color * textColor * 1.0f;
		
//		outColor = outColor * light.color * result.color * 1.0f;
		return outColor ;
}

void addObject(vec3 P, float R, vec4 C, float shinyness){
	
	objects[id].pos = P;
	objects[id].radius = R;
	objects[id].color = C;
	objects[id].shinyness = shinyness;

	id++;
}

vec4 Tracer()
{
	vec4 color = backGroundColor;
	Ray ray = genRay(vertexPos);
	intersectResult result;
	int reflectCount = 10;
	bool reflected = false;

	for(int i = 0; i < 1; i++)
	{
		if(reflectCount <= 0)//if more than 10 reflections then stop
		{
			break;
		}

		//test for intersection
		result = intersect(ray);
		if(result.hit)
		{
			//if hit an object shade it
			color = shade(result, ray);
			//if object is very shiny, gen new ray and redo intersection;
			if(result.shinyness > 40.0f )
			{
				//generate a new ray with reflection ray if the object is reflective
				ray = reflectionRay(ray, result);
				//restart loop as an itterative recursion approach
				i--;
				reflectCount--;
			}
		}
	}	

	//loop reflection

	return color;
};



void main(){

	//set up scene
	addObject(vec3(0.0,0.0, -1.0),0.1f,vec4(0.0,1.0,0.0,1.0),50.0f);
	addObject(vec3(-0.15,0.0, -0.8),0.06f,vec4(0.1, 0.7, 0.9,1.0),10.0f);
	
	//set up light
//	light.pos = vec3(-10.0,1.0,10.0);
	light.pos = vec3(5.0,1.0,5.0);
//	light.pos = vec3(10.0,1.0,-10.0);
	light.color = vec4(1.0);


	//return color is result of tracer
	fColor = Tracer();

	return;
}