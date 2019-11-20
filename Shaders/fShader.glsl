#version 430 core
out vec4 fColor;

in vec3 vertexPos;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

int id;

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
};

vec4 backGroundColor = vec4(0.0, 0.0 ,0.0,1.0);

Ray genRay(vec3 vertexPos);

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

	startPoint = projectionMatrix * startPoint;
	startPoint = viewMatrix * startPoint;
	startPoint = startPoint / startPoint.w;
	
	endPoint = projectionMatrix * endPoint;
	endPoint = viewMatrix * endPoint;
	endPoint = endPoint / endPoint.w;

	ray.origin = vec3(startPoint);
	ray.direction = vec3(normalize(endPoint - startPoint));

	return ray;
}

Ray reflectionRay(Ray ray, intersectResult result){

		//intersection point
	vec3 intersect = ray.origin + (result.dist)*normalize(ray.direction);
	vec3 surfaceNormal = normalize(intersect - result.pos);

	vec3 newRayDir = (-ray.direction)+2*(dot(ray.direction,surfaceNormal))*surfaceNormal;

	ray.direction = newRayDir*-1;
	ray.origin = (intersect + (newRayDir*0.0001f));

	return ray;
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
			}
		}
	}

return rtn;
}

vec4 shade(intersectResult result, Ray ray){
		
		vec3 n = normalize(ray.direction);

		//intersection point
		vec3 intersect = ray.origin + (result.dist)*n;

		//get surface normal
		vec3 surfaceNormal = normalize(intersect - result.pos);

		//SHADING Phong
			
		light.direction = normalize(light.pos - intersect);
		vec3 viewDir = normalize(vertexPos - intersect);
		vec3 midDir = normalize(light.direction + viewDir);


		//phong shading model // needs to be updated with pbr
			
		float ambientStr = 0.1f;

		vec4 ambient = vec4(ambientStr);
		vec4 diffuse = vec4(max(dot(surfaceNormal, light.direction),0.0f));

		vec4 specular = vec4(facing(surfaceNormal, light.direction) * pow(max(dot(surfaceNormal, midDir), 0.0f), result.shinyness));

		vec4 outColor = ambient + diffuse + specular; // reflectivity

		outColor = outColor * light.color * result.color * 1.0f;
		
		return outColor / 3.0f;
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
				//reflected = true;
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
	addObject(vec3(-0.2,0.0, -0.9),0.1f,vec4(0.0,1.0,0.0,1.0),10.0f);
	
	//set up light
	light.pos = vec3(-10.0,1.0,10.0);
	light.color = vec4(1.0);


	//return color is result of tracer
	fColor = Tracer();

	return;
}