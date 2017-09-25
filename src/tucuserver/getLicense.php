<!DOCTYPE html>
<html lang="en">

<head>
  <meta charset="UTF-8">
  <title>Tucuxi License Server</title>

  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-alpha.6/css/bootstrap.min.css" integrity="sha384-rwoIResjU2yc3z8GV/NPeZWAv56rSmLldC3R/AZzGRnGxQQKnKkoFVhFQhNUwEyJ" crossorigin="anonymous">
</head>

<body>

	<div class="container" >
		<br/>
		
		<div class="jumbotron">
			<table>
				<tr>
					<td width='150px' ><img src="Tucuxi.png" style="vertical-align:bottom"/></td>
					<td><h1 class="display-3">Tucuxi License Server</h1></td>
				</tr>
			</table>
			<br/>
<?php

$servername = "localhost";
$username = "tuculicense";
$password = "tucuxi";
$dbname = "tuculicense";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check connection
if ($conn->connect_error) {
	die("Connection failed: " . $conn->connect_error);
}

// Build data
$date = date("Y-m-d H:i:s");
$request = htmlspecialchars($_GET["request"]);
$firstName =  htmlspecialchars($_GET["firstName"]);
$lastName =  htmlspecialchars($_GET["lastName"]);
$title =  htmlspecialchars($_GET["title"]);
$company =  htmlspecialchars($_GET["company"]);
$email =  htmlspecialchars($_GET["email"]);
$duration = 60;
$error = "";

// Create license
exec("/var/www/license/tuculicense " . $request . " " . $duration . " 2>&1", $license);

// Check result
if (count($license) != 1) {
	$error =  "Error generating license 1";
}
if (strpos($license[0], 'Error') !== false) {
	$error = "Error generating license 2";
}

// Insert record into our database
if ($error == "") {
	$sql = "INSERT INTO requests (date, firstname, lastname, title, company, email, request, duration, license) VALUES ('" .$date. "', '" . $firstName . "', '" . $lastName . "', '" . $title . "', '" . $company . "', '" . $email . "', '" .$request . "', " . $duration . ", '" . $license[0] . "')";
	if ($conn->query($sql) !== TRUE) {
	    $error =  "Error: " . $sql . "<br>" . $conn->error;
	}
}

// Display the generated license
if ($error == "") {
?>
			<p class="alert alert-success" role="alert">Your Tuxuci License is ready!</p> <h3>
	<p>
	<textarea class="form-control" readonly>
<?php
	echo $license[0];
?>	
	</textarea>
	</p>
<?php
}
else {
?>
			<p class="alert alert-danger" role="alert"> 
<?php
	echo $error;
?>
			</p> <hr class="my-4">
			<p>Please contact our technical support for help: <a href="mailto:support@tucuxi.ch">support@tucuxi.ch</a></p>
<?php
}
?>
		</div>

	<script src="https://code.jquery.com/jquery-3.1.1.slim.min.js" integrity="sha384-A7FZj7v+d/sdmMqp/nOQwliLvUsJfDHW+k9Omg/a/EheAdgtzNs3hpfag6Ed950n" crossorigin="anonymous"></script>
	<script src="https://cdnjs.cloudflare.com/ajax/libs/tether/1.4.0/js/tether.min.js" integrity="sha384-DztdAPBWPRXSA/3eYEEUWrWCy7G5KFbe8fFjk5JAIxUYHKkDx6Qin1DkWx51bBrb" crossorigin="anonymous"></script>
	<script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-alpha.6/js/bootstrap.min.js" integrity="sha384-vBWWzlZJ8ea9aCX4pEW3rVHjgjt7zpkNpZk+02D9phzyeVkE+jo0ieGizqPLForn" crossorigin="anonymous"></script>
</body>

</html>
