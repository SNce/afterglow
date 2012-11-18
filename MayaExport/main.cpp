#define _BOOL
#define REQUIRE_IOSTREAM

#include "maya\MSimple.h"
#include "maya\MFileIO.h"
#include "maya\MStatus.h"
#include "maya\MLibrary.h"
#include "maya\MDagPath.h"
#include "maya\MItDag.h"
#include "maya\MGlobal.h"
#include "maya\MFnDagNode.h"
#include "maya\MFnMesh.h"
#include "maya\MPointArray.h"
#include "maya\MItMeshPolygon.h"
#include "maya\MFloatVectorArray.h"

#define BINARY

#include <fstream>

void PrintHelp();

void main(int args, char** argv)
{
	bool bVerbose = false;

	if(args < 2 || !strcmp(argv[1], "--help") || !strcmp(argv[1], "--?"))
   {
	   PrintHelp();
	   exit(0);
   }

	if(!strcmp(argv[1], "-v"))
	{
		bVerbose = true;
	}

   MStatus stat = MLibrary::initialize ( "myExporter" );
   if ( !stat )
   {
	  std::cout<<"\nError: Could not initialize Maya libraries\n";
      return;
   }

   char outfileName[100];

   // buffer to store vertex and polygon data
   char* vertexBuf;
   char* polyBuf;

   // counts
   unsigned int vCount;
   unsigned int pCount;
   unsigned int quadCount;
   unsigned int triCount;

   // prepare Maya to read a new scene file
   MFileIO::newFile( true );

   // read the scene file
   if(!bVerbose)
	   stat = MFileIO::open( argv[1] );
   else
	   stat = MFileIO::open( argv[2] );
   if ( !stat )
   {
	   if(!bVerbose)
			std::cout<<"Error: Could not find the file "<<argv[1]<<std::endl;
	   else
		   std::cout<<"Error: Could not find the file "<<argv[2]<<std::endl;
      return;
   }

   // remove any temporary data created by Maya while loading
   // things like the undo list which we won't be using
   stat = MGlobal::executeCommand( "delete -ch" );
   if ( !stat )
      return;

   std::ofstream outFile;

   if(!bVerbose)
   {
	   strncpy(outfileName, argv[1], strlen(argv[1]) - 2);
	   outfileName[strlen(argv[1]) - 2] = '\0';
	   strncat(outfileName, "msh", 3);
   }
   else
   {
	   strncpy(outfileName, argv[2], strlen(argv[2]) - 2);
	   outfileName[strlen(argv[2]) - 2] = '\0';
	   strncat(outfileName, "msh", 3);
   }

#ifdef BINARY
   outFile.open(outfileName, ios::out | ios::binary);
#else
   outFile.open(outfileName, ios::out);
#endif

   // iterate through all the nodes in the DAG, and find vertex data
   MItDag dagIter( MItDag::kBreadthFirst, MFn::kInvalid, &stat );
   for ( ; !dagIter.isDone(); dagIter.next())
   {
      MDagPath dagPath;
      stat = dagIter.getPath( dagPath );

      if(stat)
	  {
		  MFnDagNode dagNode(dagPath, &stat);

		  if( dagNode.isIntermediateObject()) continue;
		  if( dagPath.hasFn(MFn::kTransform)) continue;
		  if( !dagPath.hasFn(MFn::kMesh)) continue;

		  MFnMesh fnMesh( dagPath);

		  // Get vertices
		  MPointArray vertexList;
		  fnMesh.getPoints(vertexList, MSpace::kObject);

		  /*std::cout<<"Name: "<<fnMesh.name()<<std::endl;*/

		  // Get normals
		  MFloatVectorArray normalList;
		  fnMesh.getVertexNormals(false, normalList, MSpace::kObject); 

		  vCount = vertexList.length();
#ifdef BINARY
		  outFile.write((char*)&vCount, sizeof(unsigned int));
#else
		  outFile<<vCount;
#endif

		  for(unsigned int i = 0; i < vCount; ++i)
		  {
			  vertexList[i].cartesianize();
			  normalList[i].normalize();

			  MPoint point = vertexList[i];
			  
			  float vertexnormal[6];
			  vertexnormal[0] = (float)point.x;
			  vertexnormal[1] = (float)point.y;
			  vertexnormal[2] = (float)point.z;
			  vertexnormal[3] = normalList[i].x;
			  vertexnormal[4] = normalList[i].y;
			  vertexnormal[5] = normalList[i].z;

#ifdef BINARY
			  outFile.write((char*)vertexnormal, sizeof(float) * 6);
#else
			  for(int k = 0; k < 6; k++)
				  outFile<<vertexnormal[k];
#endif
			  if(bVerbose)
			  {
				  std::cout<<"\nVertex:\n"<<vertexnormal[0]<<" "<<vertexnormal[1]<<" "<<vertexnormal[2]<<std::endl;
				  std::cout<<"Normal:\n"<<vertexnormal[3]<<" "<<vertexnormal[4]<<" "<<vertexnormal[5]<<std::endl;
			  }
		  }

		  // Get polygons
		  MItMeshPolygon piter(dagPath);
		  pCount = piter.count();

		  quadCount = 0;
		  triCount = 0;

		  for(; !piter.isDone(); piter.next())
		  {
				MIntArray vertexIdxs;
				piter.getVertices(vertexIdxs);

				//Convert quads into triangles
				if(vertexIdxs.length() == 4)
					++quadCount;
				if(vertexIdxs.length() == 3)
					++triCount;
		  }

		  // Polygon count after converting quads to tris
		  pCount = quadCount * 2 + triCount;
		  if(bVerbose)
			std::cout<<"\n Writing pcount: "<<pCount<<std::endl;

#ifdef BINARY
		  outFile.write((char*)&pCount, sizeof(unsigned int));
#else
		  outFile<<pCount;
#endif
		  if(bVerbose)
			std::cout<<std::endl<<"Polygons!!!!"<<std::endl;

		  piter.reset();
		  for(; !piter.isDone(); piter.next())
		  {
				MIntArray vertexIdxs;
				piter.getVertices(vertexIdxs);

				//Convert quads into triangles
				if(vertexIdxs.length() == 4)
				{
					int tri[6];
					tri[0] = vertexIdxs[0];
					tri[1] = vertexIdxs[1];
					tri[2] = vertexIdxs[2];
					tri[3] = vertexIdxs[0];
					tri[4] = vertexIdxs[2];
					tri[5] = vertexIdxs[3];

#ifdef BINARY
					outFile.write((char*)tri, sizeof(int) * 6);
#else
					for(int k = 0; k < 6; k++)
						outFile<<tri[k];
#endif
					if(bVerbose)
					{
						std::cout<<"Polygon :"<<tri[0]<<" "<<tri[1]<<" "<<tri[2]<<std::endl;
						std::cout<<"Polygon :"<<tri[3]<<" "<<tri[4]<<" "<<tri[5]<<std::endl;
					}
				}
				// Already a triangle
				else if(vertexIdxs.length() == 3)
				{
					int tri[3];
					tri[0] = vertexIdxs[0];
					tri[1] = vertexIdxs[1];
					tri[2] = vertexIdxs[2];

#ifdef BINARY
					outFile.write((char*)tri, sizeof(int) * 3);
#else
					for(int k = 0; k < 3; k++)
						outFile<<tri[k];
#endif
					if(bVerbose)
						std::cout<<"Polygon :"<<tri[0]<<" "<<tri[1]<<" "<<tri[2]<<std::endl;
				}
		  }
	  }	
   }

   outFile.close();

   std::cout<<"\n Summary\n-------------\n";
   std::cout<<"Successfully exported "<<argv[1]<<std::endl;
   std::cout<<"Output file: "<<outfileName<<std::endl;
   std::cout<<"Number of vertices: "<<vCount<<std::endl;
   std::cout<<"Number of quads in original mesh: "<<quadCount<<std::endl;
   std::cout<<"Number of tris in original mesh: "<<triCount<<std::endl;
   std::cout<<"Number of output polygons (tris): "<<pCount<<std::endl;
	
   // now shut down Maya, or if you want to process another file,
   // just make another call to MFileIO::newFile(), and MFileIO::open()
   MLibrary::cleanup();
}

void PrintHelp()
{
	std::cout<<"MayaExporter\n---------------------\nUsage: MayaExporter.exe <file>\nOptions:\n\t--help\\--?\t\tHelp\n\t-v\t\t\tVerbose\n";
}
