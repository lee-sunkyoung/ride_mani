#include "../inc/node.h"

Serial Node::s;
bool Node::serialRead;
mutex Node::m;
queue<uint8_t> Node::readByte;

Node::Node(int &argc, char **argv) : test(&s, &serialRead, &m, &readByte)
{
  // ros init
  ros::init(argc, argv, "jdk_stm");
  ros::NodeHandle n;

  sub = n.subscribe("/hello", 10, &Test::myCallBack, &test);
  sub_autorace = n.subscribe("/bye", 10, &Test::myCallBack_autorace, &test);


  // serial init
  s.setPort("/dev/" + portname);
  s.setBaudrate(baudrate);

  // 포트 존재하지 않거나 할 경우 대비한 예외처리
  try
  {
    s.open();
  }
  catch (serial::IOException e)   // 퍼미션, 포트 없음 등으로 열수 없을 때 예외 출력하고 리턴
  {
    cerr << "Port open failed." << e.what() << endl;
    return;
  }

  cout<<"port opened"<<endl;

  // 수신 루프 정지를 위한 변수. true이면 계속 수신하다가 false가 되면 루프 종료
  thread readThread(this->serialReadThread);
  serialRead = true;
  run();  // 메인 루프. ctrl+c 등으로 멈추면 리턴한다.
  serialRead = false; // 스레드 join울 위해 false로
  readThread.join();  // 수신 루프 종료 기다려 join
}

Node::~Node()
{
  if (ros::isStarted())
  {
    ros::shutdown();
    ros::waitForShutdown();
  }
}

void Node::run()
{
  ros::Rate loopRate(rosrate);

  for(int i = 0; i < startdelay; i++);

  while (ros::ok())
  {
    timerCallback();
    ros::spinOnce();
    loopRate.sleep();
  }
}

bool Node::retry()
{
  if (!s.isOpen())
  {
    try
    {
      s.open();
    }
    catch (exception e)
    {
      return false;
    }
  }

  return true;
}

void Node::timerCallback()
{
  if(!retry())
  {
    return;
  }

  test.Algorithm_Test();
}

void Node::serialReadThread()
{
  // 수신한 바이트를 저장할 버퍼
  unsigned char readBuffer[readbuffersize];

  // 수신 루프
  while(serialRead)
  {
    try // 중간에 통신선이 뽑히거나 할 경우 대비해 예외처리
    {
      int size = s.available();

      if(size != 0)  // 받은 바이트 수가 0이 아닐때
      {
        s.read(readBuffer, size);   // 버퍼에 읽어옴

        for(int i = 0; i < size; i++)
        {
          m.lock();   // producer consumer pattern. 뮤텍스 lock
          readByte.push(readBuffer[i]);
          m.unlock(); // 뮤텍스 unlock
        }
      }
    }
    catch (IOException e)   // 예외 발생시 메시지 띄우고 포트 닫는다.
    {
      cerr << "Port disconnected. closing port(" << e.what() << ")." << endl;
      s.close();
    }

    this_thread::sleep_for(chrono::milliseconds(1));    // cpu 점유율 낮추기 위해 잠깐 sleep
  }
}

