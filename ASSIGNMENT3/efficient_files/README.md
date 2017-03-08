# Communication between multiple clients via server and database
<br>
	<h3>Software Setup :</h3>
	<p> First we have to set up our system to use all those necessary tools.</p>
	<ul>
		<li> If sqlite3 is not installed then Download <a href="http://www.sqlite.org/download.html" target = "_blank">sqlite-autoconf-3170000.tar.gz</a> Install it by following the below steps</li>
		<ul>
			<li> tar xvfz sqlite-autoconf-3170000.tar.gz</li>
			<li>cd sqlite-autoconf-3170000</li>
			<li>./configure --prefix=/usr/local </li>
			<li> sudo make </li>
			<li> sudo make install</li>
		</ul>
	</ul>
	<h3> Objective : </h3>
	<p> There will be multiple clients and every two clients can chat with each other. Server will have a database and the all the messages will get stored into the database.
	</p>
	<h3> Model : </h3>
	<p>
		The schema of the database -> Each entry has four fields : 
		<ul>
			<li>USER-ID</li>
			<li>USER-NAME</li>
			<li>DATA</li>
			<li>STATUS</li>
		</ul>
	</p>
	<h3> Algorithm : </h3>
	<ul>
		<h4> Server Side tasks: </h4>
		<li>
			<p> For each client server will create a thread. Each thread corresponding to one client will spawn two threads , <strong> 1. For reading from client socket and to write them into database --- 2. For reading from database and writing to the corresponding client socket.</strong></p>
		</li>
	</ul>
</br>