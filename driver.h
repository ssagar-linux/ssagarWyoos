
#ifndef __DRIVER_H
#define __DRIVER_H

	class Driver
	{
		public:
			Driver();
			~Driver();

			virtual void Activate();
			virtual int Reset();
			virtual void Deactivate();
	};


	class DriverManager: public Driver
	{
		private:
			Driver* ppDrivers[265];
			int numDrivers;

		public:
			DriverManager();
			void AddDriver(Driver*);

			void ActivateAll();
	};

#endif
