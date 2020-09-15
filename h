const express = require("express");
const app = express();
const port = 3000;
const morgan = require("morgan");
const cors = require("cors");
const mongoClient = require("mongodb").MongoClient;
const objectID = require("mongodb").ObjectID;
const bcrypt = require("bcrypt");
const bodyParser = require("body-parser");
const validator = require("validator");

//Middleware
app.use(morgan("tiny"));
app.use(cors());
app.use(bodyParser.json());

app.listen(port, () =>
  console.log(`Server listening at http://localhost:${port}`)
);

mongoClient
  .connect(
    "mongodb+srv://admin:iy9IdwCPawjukzhy@crud-mongo-express-bcorw.mongodb.net/test?retryWrites=true&w=majority",
    { useUnifiedTopology: true }
  )
  .then((client) => {
    console.log("Connected to Database");
    const db = client.db("db");
    const users = db.collection("users");

    app.post("/signup", (req, res) => {
      users.findOne({ username: req.body.username }).then((result) => {
        if (!result) {
          if (validator.isLength(req.body.username, { min: 3, max: 20 })) {
            if (validator.isAlphanumeric(req.body.username, ["en-GB"])) {
              if (validator.isEmail(req.body.email)) {
                users.findOne({ email: req.body.email }).then((result) => {
                  if (!result) {
                    if (
                      validator.isLength(req.body.password, { min: 3, max: 20 })
                    ) {
                      if (
                        validator.equals(
                          req.body.password,
                          req.body.repeatPassword
                        )
                      ) {
                        bcrypt.genSalt(8, (err, salt) => {
                          bcrypt.hash(req.body.password, salt, (err, hash) => {
                            users
                              .insertOne({
                                username: req.body.username,
                                email: req.body.email,
                                password: hash,
                                avatar: "https://steamuserimages-a.akamaihd.net/ugc/885384897182110030/F095539864AC9E94AE5236E04C8CA7C2725BCEFF/",
                                banner: "https://www.acemetrix.com/wp-content/themes/acemetrix/images/default/default-black-banner.png"
                              })
                              .then((result) => {
                                res.send(result);
                              });
                          });
                        });
                      } else res.status(400).send("Password doesnt match");
                    } else
                      res.status(400).send(
                        "Password needs to be beetween 3 and 20 characters"
                      );
                  } else res.status(400).send("Email already exists");
                });
              } else res.status(400).send("Not a valid email address");
            } else res.status(400).send("Username can only contains letters and numbers");
          } else res.status(400).send("Username needs to be beetween 3 and 20 characters");
        } else res.status(400).send("Username already exists");
      });
    });

    app.post("/login", (req, res) => {
        users.findOne({username: req.body.username})
        .then(result => {
            if (result) {
                bcrypt.compare(req.body.password, result.password, (err, equal) => {
                    if (!equal) res.status(400).send("Wrong password!")
                    else {
                        res.send(result)
                    }
                });
            }
            else res.status(400).send("That username doesnt exist")
        })
    })
  })
  .catch((error) => console.error(error));
