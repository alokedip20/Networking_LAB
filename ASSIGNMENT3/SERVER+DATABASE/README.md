# Simple chat application using Mulithreaded Socket programming in C++
<h2>Objective : </h2>
<p>
	In this project there will be one server and multiple clients. Among those clients any two clients can chat with each other. Server will have a sqlite database into which all informations corresponfing those clients will be stored.
</p>
<h2>Model : </h2>
<p>
	The model will consist of one server and one database. Only server has the privilage to access the database and it only can manipulate the database. In this particular project there were two clients. In the server when a client will make a successfull connection then server will create a thread and that child thread will do all those necesary jobs for the clients in future till the partiular client will be connected with the server. That thread will create two threads to perform these following tasks:
			<ul>
				<li> One thread will read from the client socket and update the database.
				</li>
				<li> Another thread will retrieve data for a client and peform some short of filtering to prevent sending duplicate messages and after doing this it will write the filrered data to the particular client socket.</li>
			</ul>
</p>