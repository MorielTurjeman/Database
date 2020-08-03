/*
 * Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#pragma comment(lib, "mysqlcppconn8.lib")

#include <iostream>
#include <mysqlx/xdevapi.h>
#include <string>
#include <ctime>
#include <vector>
using ::std::cout;
using ::std::endl;
using namespace ::mysqlx;

//query 13, shipment status
void getShipmentStatus(Session &sess, int shipmentId)
{
	auto query = sess.sql(R"(select shipments.shipment_id, shipmentStatus
				from
				bookstore.shipments where shipment_id = ?)");
	query.bind(shipmentId);
	auto set = query.execute();
	auto row = set.fetchOne();
	if (row)
	{
		// the query returned at least one row
		cout << "The status of shipment id: " << shipmentId << " is: " << row.get(1) << endl;
		//fetch one return the current row, and advances to the next one, if exists.
		//we use fetchone instead of a loop since this query should return only one row anyway,
		//if more than one row should be reutrned, we will use a loop with fetchall.
		//get(1) returns the column index 1 in the row
	}
	else
	{
		cout << "Could not find shipment" << endl;
		// the query returnes 0 rows, or in our case, the shipment id is wrong
	}
}

//query 3, The oldest book in stock
void oldestBook(Session &sess)
{
	auto query = sess.sql(R"(select books.bookName
from        store_purchase
                inner join books on books.book_id = store_purchase.book_id
                where
                        store_purchase_id not in (select books_in_shipments.store_purchase_id from books_in_shipments)
order by purchaseDate
limit 1;)");

	auto set = query.execute();
	auto row = set.fetchOne();
	if (row)
	{
		cout << "the oldest book is:" << row.get(0) << endl;
	}

	else
	{
		cout << "No books at Data Base" << endl;
	}
}

//query 5, how many copies of book x has been sold

void copiesSold(Session &sess, std::string bookName)
{
	auto query = sess.sql(R"(select books.book_id, books.bookName, count(books.book_id)
							from         books_in_shipments
							inner join store_purchase on books_in_shipments.store_purchase_id = store_purchase.store_purchase_id
							inner join books on store_purchase.book_id = books.book_id
							where         books.bookName = ?
							group by
							books.book_id)");

	query.bind(bookName);
	auto set = query.execute();
	auto row = set.fetchOne();
	if (row)
	{
		cout << "The book:" << bookName << " has been sold: " << row.get(2) << " times" << endl;
	}

	else
	{
		cout << "The book have never been sold" << endl;
	}
}

//query 1, is book X in stock

// void isBookInStock(Session& sess, std::string bookName)
// {
// 	//define query
// 	auto query = sess.sql(R"(SELECT
//     b.bookName,
//     b.book_id,
//     count(sp.store_purchase_id) AS 'stockCount',
//     count(bis.store_purchase_id) AS 'soldCount',
//     ('stockCount' - 'soldCount') as toal
// FROM
//     books AS b
//         JOIN
//     store_purchase AS sp ON b.book_id = sp.book_id
//         JOIN
//     books_in_shipments AS bis ON sp.store_purchase_id = bis.store_purchase_id
// WHERE
//     b.bookName = ?

// GROUP BY b.book_id;)");

// 	query.bind(bookName); // set the paramenters
// 	auto set = query.execute(); //run the query
// 	if (set.hasData())
// 	{
// 		// the query returned at least one row
// 		cout << "There are " << set.fetchOne().get(4) << " copies of " << bookName << endl;

// 	}
// 	else
// 	{
// 		cout << "Could not get number of copies" << endl;
// 		// the query returnes 0 rows
// 	}
// 	set.count();
// 	auto rows = set.fetchAll();

// 	for (auto row : rows)
// 	{
// 		std::cout << row.get(0) << ", " << row.get(1) << ", " << row.get(2);
// 	}

// }

//query 2, who is the oldest customer
void whoIsTheOldestCustomer(Session &sess)
{
	//define query
	auto query = sess.sql(R"(
	select concat(firstName, " ", lastName) as fullname from bookstore.customer where joinDate = (select min(joinDate) from bookstore.customer);
	)");

	auto set = query.execute(); //run the query
	auto row = set.fetchOne();
	if (row)
	{
		// the query returned at least one row
		cout << "The oldest customer is: " << row.get(0) << std::endl;
	}
	else
	{
		cout << "Could not find the oldest customer. Please make sure there are customers in the Database." << std::endl;
		// the query returnes 0 rows
	}


}

//query 4, current reservation list

void reservationList(Session &sess)
{
	auto query = sess.sql(R"(
		SELECT customer.firstName, customer.lastName, books.bookName,   DATE_FORMAT(reservations.reservationDate, "%Y-%M-%d") , reservations.reservationStatus
		FROM
			bookstore.reservations
		INNER JOIN
			customer on reservations.customer_id= customer.customer_id
		INNER JOIN
			books on reservations.book_id=books.book_id
		WHERE
			reservationStatus = 'ordered'
			OR reservationStatus = 'arrived to store'
			ORDER BY reservationDate DESC; 
	)");

	auto set = query.execute(); //run the query
	

		std::list<Row> rows = set.fetchAll();
		if (rows.size() > 0)
		{
			for (auto row : rows)

			{
				
				
					std::cout << "Customer:" << row.get(0) << "  " << row.get(1) << " , "  "book name:" << row.get(2) << ", " << "reservation date: " << row.get(3) << " " << "Statuse: " << row.get(4) << endl;
				
				std::cout << endl;
			}
		}
		else
		{
			cout << "Could not find any reservations." << std::endl;
		}
	
	
}

//query 7, 3 customers who have bought the most books
void top3Customers(Session &sess)
{
	auto query = sess.sql(R"(
	select firstName, lastName, count(*) as 'Total orders'
	from purchase p
	join customer c on p.customer_id = c.customer_id
	group by p.customer_id
	order by 'Total orders' asc limit 3;

	)");

	auto set = query.execute();

	std::vector<Row> rows = set.fetchAll();

	if (rows.size())
	{
		
		for (auto row : rows)
		{
			for (int i = 0; i < 3; i++)
			{
				std::cout << row.get(i) << " , ";
			}
			std::cout << std::endl;
		}
	}
	else
	{
		std::cout << "No customers found." << std::endl;
	}
}

//query 8, the book with most traslations
void mostTranslatedBook(Session &sess)
{
	auto query = sess.sql(R"(select bookName
	from (select bookName, count(bookName) as Translations
	from books group by bookName) as a
	group by bookName
	order by  Translations desc limit 1
	)");
	auto set = query.execute();
	auto row = set.fetchOne();
	if (row)
	{
		std::cout << "The most translated book in stock is: " << row.get(0) << std::endl;
	}

	else
	{
		std::cout << "No books found." << std::endl;
	}
}

//query 11, calculate shipping

void calculateShipping(Session &sess, int shipment_id)
{
	auto query = sess.sql(R"(select deliveryID, (DeliveryCost * PercelWeight) as ShipmentCost
 from (select ship.shipment_id as deliveryID, ship.purchase_id as OrderID , ship.delivery_type_id as DeliveryType, boo.weight as 'PercelWeight',
  de.weightCost as DeliveryCost, boo.book_id as BookID from shipments ship 
   inner join books_in_shipments bis
    on ship.shipment_id = bis.shipment_id
     inner join store_purchase sp
      on sp.store_purchase_id=bis.store_purchase_id
       inner join delivery_types de 
         on ship.delivery_type_id = de.delivery_type_id
         inner join books as boo on sp.book_id = boo.book_id) as temp
         where deliveryID = ?
                                order by deliveryID asc;)");
	query.bind(shipment_id);
	auto set = query.execute();
	auto row = set.fetchOne();
	if (row)
	{
		std::cout << "The shipping price for the delivery is: " << row.get(1) << " New Israeli Shekels." << std::endl;
	}
	else
	{
		std::cout << "Shipping cost could not be calculated, please make sure you entered the correct shipping id and that there are books in the Database" << std::endl;
	}
}

//query 1, is book in stock
void isBookInStock(Session &sess, std::string bookName)
{

	//define query
	auto query = sess.sql(R"(SELECT 
   b.bookName,
   b.book_id,
   count(sp.store_purchase_id) AS 'stockCount',
   count(bis.store_purchase_id) AS 'soldCount',
   ('stockCount' - 'soldCount') as toal
FROM
   books AS b
       JOIN
   store_purchase AS sp ON b.book_id = sp.book_id
       JOIN
   books_in_shipments AS bis ON sp.store_purchase_id = bis.store_purchase_id
WHERE
   b.bookName = ?
       
GROUP BY b.book_id;)");

	query.bind(bookName);		// set the paramenters
	auto set = query.execute(); //run the query
	auto rows = set.fetchAll();

	for (auto row : rows)
	{
		std::cout << row.get(0) << ", " << row.get(1) << ", " << row.get(2);
	}
}

//query 6, Favorite Author
void favAuthor(Session &sess, std::string purchaseDateD1, std::string purchaseDateD2)
{
	auto query = sess.sql(R"(select authors.firsName, authors.lastName, count(book_authors.book_id) 'readTheBookCount'
from bookstore.authors
        inner join bookstore.book_authors on book_authors.author_id= authors.author_id
		inner join bookstore.store_purchase on book_authors.book_id= store_purchase.book_id
		inner join bookstore.books_in_shipments on bookstore.store_purchase.store_purchase_id = books_in_shipments.store_purchase_id
		inner join bookstore.shipments on books_in_shipments.shipment_id =shipments.shipment_id
		inner join bookstore.purchase on shipments.shipment_id =purchase.purchase_id
		where purchasDate between str_to_date(?, '%Y-%m-%d') and str_to_date(?, '%Y-%m-%d') 
		 group by authors.author_id 
		 order by count(book_authors.book_id) desc
		limit 1;)");

	query.bind(purchaseDateD1);
	query.bind(purchaseDateD2);
	auto set = query.execute();
	auto row = set.fetchOne();
	if (row)

	{
		
		std::cout << "The author most read is: " << row.get(0) << " " << row.get(1) << endl;
	}

	else
	{
		cout << "There were no purchase at this dates, try other dates" << endl;
	}
}

//query 12, Split shipments details
void splitShipments(Session &sess, std::string customerFN, std::string customerLN)
{
	auto query = sess.sql(R"(select purchase.purchase_id, DATE_FORMAT(purchase.purchasDate, "%Y-%M-%d") , customer.firstName, customer.lastName, delivery_types.company, delivery_types.type, s1.trackingNum, s1.address
from
bookstore.shipments s1 inner join bookstore.shipments s2 on s1.purchase_id=s2.purchase_id
inner join delivery_types on s2.delivery_type_id = delivery_types.delivery_type_id
inner join purchase on s1.purchase_id = purchase.purchase_id
inner join customer on purchase.customer_id = customer.customer_id
where s1.shipment_id != s2.shipment_id and customer.firstName = ? and customer.lastName=?;)");

	query.bind(customerFN);
	query.bind(customerLN);
	auto set = query.execute();

	if (set.hasData())
	{
		auto rows = set.fetchAll();

		for (auto row : rows)
		{
			std::cout << "purchase id: ";
			for (int i = 0; i < 8; i++)
				std::cout << row.get(i) << " "; // << ", " << row.get(1) << ", " << row.get(2) << ", " << row.get(3) << ", " << row.get(4) << ", " << row.get(5) << ", " << row.get(6) << ", " << row.get(7) << endl;
			
		}
	}
	else
		cout << "The customer: " << customerFN << " " << customerLN << "have never split a shipment" << endl;
}

//query 16, Transactions with above-average profits
void aboveAvg(Session &sess)
{
	auto query = sess.sql(R"(SELECT 
    purchase.purchase_id,
        DATE_FORMAT(purchase.purchasDate, "%Y-%M-%d"),
        SUM(store_purchase.price - store_purchase.book_cost) 'totalProfit'
FROM
    books_in_shipments
        INNER JOIN
    store_purchase ON books_in_shipments.store_purchase_id = store_purchase.store_purchase_id
        INNER JOIN
    shipments ON books_in_shipments.shipment_id = shipments.shipment_id
        INNER JOIN
    purchase ON shipments.purchase_id = purchase.purchase_id
    where purchase.purchasDate > DATE_ADD(NOW(), INTERVAL - 12 MONTH)
GROUP BY purchase.purchase_id
HAVING SUM(store_purchase.price - store_purchase.book_cost) > (SELECT 
        AVG(store_purchase.price - store_purchase.book_cost) 'Average'
    FROM
        books_in_shipments
            INNER JOIN
        store_purchase ON books_in_shipments.store_purchase_id = store_purchase.store_purchase_id
            INNER JOIN
        shipments ON books_in_shipments.shipment_id = shipments.shipment_id
            INNER JOIN
        purchase ON shipments.purchase_id = purchase.purchase_id
    WHERE
        purchase.purchasDate > DATE_ADD(NOW(), INTERVAL - 12 MONTH))
ORDER BY purchase.purchase_id;
)");

	auto set = query.execute();

	if (set.hasData())
	{

		auto rows = set.fetchAll();
		std::cout << "Transactions with above-average profits in the past 12 months:" << endl;
		for (auto row : rows)
		{

			std::cout << "Purchase ID: " << row.get(0) << " , "
					  << "Purchase date: " << row.get(1) << " , "
					  << "Total profit(NIS): " << row.get(2) << endl;
		}
		std::cout << endl;
	}

	else
		std::cout << "There are no Transactions with above - average profits in the past 12 months" << endl;
}

//query 18, Shipments with two different editions
void diffEdition(Session &sess)
{

	auto query = sess.sql(R"(SELECT distinct
    shipments.shipment_id, shipments.trackingNum, shipments.address, shipments.shipmentStatus, delivery_types.company, delivery_types.type, T1.bookName
FROM
    (SELECT 
        books_in_shipments.shipment_id,
            books.book_id,
            books.bookName
    FROM
        books_in_shipments
    INNER JOIN store_purchase ON books_in_shipments.store_purchase_id = store_purchase.store_purchase_id
    INNER JOIN books ON store_purchase.book_id = books.book_id) AS T1
        INNER JOIN
    (SELECT 
        books_in_shipments.shipment_id,
            books.book_id,
            books.bookName    FROM
        books_in_shipments
    INNER JOIN store_purchase ON books_in_shipments.store_purchase_id = store_purchase.store_purchase_id
    INNER JOIN books ON store_purchase.book_id = books.book_id) AS T2 ON T1.shipment_id = T2.shipment_id
    inner join shipments on T1.shipment_id = shipments.shipment_id
    inner join delivery_types on shipments.delivery_type_id = delivery_types.delivery_type_id
WHERE
    T1.book_id != T2.book_id
        AND T1.bookName = T2.bookName;)");

	auto set = query.execute();
	if (set.hasData())
	{

		auto rows = set.fetchAll();

		for (auto row : rows)
		{

			std::cout << "Shipment ID: " << row.get(0) << ", "
					  << "Tracking Number: " << row.get(1) << ", "
					  << "Address :" << row.get(2) << ", "
					  << "Shipment Status: " << row.get(3) << ", "
					  << "Company: " << row.get(4) << ", "
					  << "Delivery type: " << row.get(5) << ", "
					  << "Book name: " << row.get(6)
					  << endl;
		}
	}
	else
	{

		std::cout << "NO shipments with two different editions";
	}
}

//query 14, sum of deliveries done by Xpress in a given month
void sumXpress(Session &sess, int month, int year)
{
	auto query = sess.sql(R"(
	select b.company,  count(*) as DeliveriesCount
	 from shipments s
	 inner join  bookstore.delivery_types b
	  on s.delivery_type_id = b.delivery_type_id
	   inner join bookstore.purchase p
		on s.purchase_id = p.purchase_id
	where month(purchasDate) = ?
	and year(purchasDate) = ?
	group by b.company
	having  b.company = 'Xpress';
	)");

	query.bind(month);
	query.bind(year);
	auto set = query.execute();
	auto row = set.fetchOne();
	if (row)
	{
		
		std::cout << "Sum of deliveries done by Xpress in " << month << " of " << year << ": " << row.get(1) << std::endl;
	}

	else
	{
		std::cout << "There are no deliveries done by Xpress in " << month << " of " << year << std::endl;
	}
	set.count();
	auto rows = set.fetchAll();
}

//query 15, sum of money transfered by Bit in a given month
void sumBit(Session &sess, int month, int year)
{
	auto query = sess.sql(R"(
	select month(purchasDate), paymentType, count(paymentType='Bit') as 'bit-count', sum(totalPrice)
	from bookstore.purchase where paymentType = 'Bit'
	and month(purchasDate) = ? and year(purchasDate) = ?
	group by month(purchasDate);
	)");
	query.bind(month);
	query.bind(year);
	auto set = query.execute();
	auto row = set.fetchOne();

	if (row)
	{
		auto rows = set.fetchAll();
		std::cout << "Sum of money recieved by the app 'Bit' in " << month << " of " << year << ": " << row.get(3) << " New Israeli Shekels" << std::endl;
	}

	else
	{
		std::cout << "Could not find money from Bit in " << month << " of " << year << std::endl;
	}
	set.count();
	auto rows = set.fetchAll();
}

//query 17, how many shippings were done by Xpress and how many by the post office in the last 12 months
void numOfShippingsExpressAndPost(Session& sess)
{
	auto query = sess.sql(R"(
	SELECT 
    SUM(ship.delivery_type_id BETWEEN 1 AND 2) AS postCount,
    SUM(ship.delivery_type_id BETWEEN 3 AND 4) AS xpressCount
FROM
    delivery_types AS del
        JOIN
    shipments AS ship ON del.delivery_type_id = ship.delivery_type_id
        JOIN
    purchase AS purch ON ship.purchase_id = purch.purchase_id
WHERE
    (purch.purchasDate BETWEEN DATE_SUB(NOW(), INTERVAL 12 MONTH) AND NOW());
	)");

	auto set = query.execute();
	set.count();
	auto row = set.fetchOne();
	if (row)
	{
		
		
			std::cout << "Number of deliveries done by The Israeli Post Office: " <<row.get(0) << ". Number of deliveries done by Xpress company: " << row.get(1) << std::endl;
		
	}

	else
	{
		std::cout << "Could not find how many shippings were done by Xpress and the Post Office." << std::endl;
	}
	
}

//query 19, list of customers who haven't bought anything in the past two years
void noPurchaseInThePastTwoYears(Session& sess)
{
	auto query = sess.sql(R"(
	SELECT 
    c.firstName,
    c.lastName,
    c.customer_id,
    c.address,
    c.phoneNum
FROM
    bookstore.purchase AS p
        JOIN
    bookstore.customer AS c ON p.customer_id = c.customer_id
WHERE
    p.customer_id NOT IN (SELECT DISTINCT
            purchase.customer_id
        FROM
            bookstore.purchase
        WHERE
            purchasDate BETWEEN DATE_ADD(CURRENT_TIMESTAMP,
                INTERVAL - 2 YEAR) AND CURRENT_TIMESTAMP);
	)");

	auto set = query.execute();
	set.count();
	auto rows = set.fetchAll();
	if (set.hasData())
	{
		for (auto row : rows)
		{
			std::cout << "Customers who have not made any purchase in the past 24 months: " << std::endl;
			std::cout << row.get(0) << " " << row.get(1) << ", customer id: " << row.get(2) << ", address:" << row.get(3) << ", phone number: " << row.get(4)<< "." << std::endl;
		}
		std::cout << std::endl;
	}

	else
	{
		std::cout << "Could not find how many shippings were done by Xpress and the Post Office." << std::endl;
	}

}

//query 20, customers who made reservations and haven't bought the books
void haventPurchasedReservations(Session& sess)
{
	auto query = sess.sql(R"(
	select c.firstName, c.lastName, res.customer_id, res.book_id, DATE_FORMAT(res.lastContact,"%Y-%M-%d"), DATE_FORMAT( res.reservationDate, "%Y-%M-%d")  from bookstore.reservations as res join bookstore.customer as c on res.reservation_id = c.customer_id
	where reservationStatus = 'arrived to store' 
	and lastContact <= date_add(current_timestamp, interval -14 day);
	)");

	auto set = query.execute();
	set.count();
	auto rows = set.fetchAll();
	if (set.hasData())
	{
		for (auto row : rows)
		{
			std::cout << "Customers who have made reservations, were notified the books arrived and haven't bougt: " << std::endl;
			std::cout << row.get(0) << " " << row.get(1) << ", customer id: " << row.get(2) << ", book id:" << row.get(3) << ", last contacted: " << row.get(4) << ", reservation date: "<< row.get(5) << std::endl;
		}
		std::cout << std::endl;
	}

	else
	{
		std::cout << "Could not find how many shippings were done by Xpress and the Post Office." << std::endl;
	}
}

//query 22, how many books the store purchase between date1 and date 2, and the sum of the price for them
void storePurchaseBetweenTwoDates(Session& sess, std::string purchaseDate1, std::string purchaseDate2)
{
	auto query = sess.sql(R"(
	select count(store_purchase_id) as purchaseCount,
	sum(book_cost)
	from store_purchase where purchaseDate between str_to_date(?, '%Y-%m-%d') and str_to_date(?, '%Y-%m-%d');
	)");
	query.bind(purchaseDate1);
	query.bind(purchaseDate2);

	auto set = query.execute();
	auto row = set.fetchOne();
	if (row)
	{
		
		
			std::cout << "Number of books the store bought: " << row.get(0) << "."<<std::endl;
			std::cout << "Total cost for these books: " << row.get(1) <<"."<< std::endl;
		
		std::cout << std::endl;
	}

	else
	{
		std::cout << "Could not find books between the two dates." << std::endl;
	}
}

//query 24, salary of a given employee
void calculateSalary(Session& sess, int employeeId, int month, int year)
{
	auto query = sess.sql(R"(
	SELECT 
    emp.idEmployees as 'employee id',
    emp.firstName,
    emp.lastName,
    wh.hour_month_salary AS 'wage per hour at the time',
    (wh.hour_month_salary * wh.hours) AS 'total-salary',
    wh.month,
    wh.year
FROM
    employees AS emp
        JOIN
    work_hours AS wh ON emp.idEmployees = wh.employee_id
WHERE
    emp.idEmployees = ? AND wh.month = ?
        AND wh.year = ?;
	)");

	query.bind(employeeId);
	query.bind(month);
	query.bind(year);

	auto set = query.execute();
	auto row = set.fetchOne();

	if (row)
	{
		
		
			std::cout << "Salary of " << row.get(1) << " " << row.get(2) << " for month:" <<month <<" "<<"is:" << row.get(4) << "." << std::endl;
		
		std::cout << std::endl;
	}
	else
	{
		std::cout << "Could not calculate salary of employee with employee id" << employeeId << ". Please make sure the number you entered is correct." << std::endl;
	}
}


//query 21, books in storage in monthly view
void monthlyStorage(Session& sess, int year)
{
	auto query = sess.sql(R"(select month, sum(bookIntoStorage) from
(
select distinct month(purchaseDate) as 'month', count(*) over(order by month(purchaseDate))  as 'bookIntoStorage'
from
	store_purchase
where
	location = 'storage'
    and
    year(purchaseDate) = ?
union 
select month(purchasDate), -1*count(*) as 'booksOutOfStorage'
from books_in_shipments inner join shipments on shipments.shipment_id=books_in_shipments.shipment_id
inner join purchase on shipments.purchase_id=purchase.purchase_id
inner join store_purchase on books_in_shipments.store_purchase_id=store_purchase.store_purchase_id
where year(purchasDate)=? and location='storage'
group by month(purchasDate)
) T1
group by T1.month
order by T1.month;)");

	query.bind(year);
	query.bind(year);
	auto set = query.execute();
	std::vector<Row> rows = set.fetchAll();

	if (rows.size())
	{
		for (auto row : rows)
			std::cout << "Year: " << year << " , Month: " << row.get(0) << ", Books in storage: " << row.get(1) << endl;
		
	}

	else
	{
		std::cout << "There were no books at storage at" << " " << year << " pick Another year " << endl;
	}

}





//query 22, Store profit in a specific month
void monthProfit(Session& sess, int year, int month) {
	auto query = sess.sql(R"(SELECT 
    t1.year, t1.month, SUM(outgoing) 'profit'
FROM
    (SELECT 
         year(store_purchase.purchaseDate) 'year', MONTH(store_purchase.purchaseDate) 'month',
            - 1 * SUM(store_purchase.book_cost) 'outgoing'
    FROM
        store_purchase
    WHERE
        YEAR(purchaseDate) = ? and
        month(purchaseDate) = ?
    GROUP BY year(purchaseDate), MONTH(purchaseDate) 
    UNION
    SELECT 
        year(purchase.purchasDate) 'year',
		MONTH(purchase.purchasDate) 'month',
            SUM(store_purchase.price) 'incoming'
    FROM
        books_in_shipments
    INNER JOIN store_purchase ON books_in_shipments.store_purchase_id = store_purchase.store_purchase_id
    INNER JOIN shipments ON books_in_shipments.shipment_id = shipments.shipment_id
    INNER JOIN purchase ON shipments.purchase_id = purchase.purchase_id
    WHERE
        YEAR(purchaseDate) = ? and
        month(purchaseDate) = ?
    GROUP BY year(purchase.purchasDate), MONTH(purchase.purchasDate)) t1
 group by t1.year, t1.month)");

query.bind(year);
query.bind(month);
query.bind(year);
query.bind(month);

auto set = query.execute();
auto row = set.fetchOne();
if (row)
	{
		
			std::cout << "year:" <<year<< ", "<< "Month: " <<  ", " << "Profit:  " << row.get(2)<< endl;
		
		
	}

	else
	{
		std::cout << "there are no profits at:" << year<< " " << month << " " << "try differnt year or month"<< endl;
	}




}



//query 23, Average transactions monthly view
void avgTrans(Session& sess, int year)
{
	auto query = sess.sql(R"(SELECT
		avg(delivery_types.costPerBook + delivery_types.weightCost * books.weight + store_purchase.price) 'transaction avg', month(purchase.purchasDate), year(purchase.purchasDate)
		FROM
		books_in_shipments
		INNER JOIN
		store_purchase ON books_in_shipments.store_purchase_id = store_purchase.store_purchase_id
		INNER JOIN
		shipments ON books_in_shipments.shipment_id = shipments.shipment_id
		INNER JOIN
		delivery_types ON shipments.delivery_type_id = delivery_types.delivery_type_id
		INNER JOIN
		books ON store_purchase.book_id = books.book_id
		INNER JOIN purchase ON shipments.purchase_id = purchase.purchase_id
		WHERE year(purchase.purchasDate) = ?
		group by year(purchase.purchasDate),month(purchase.purchasDate);)");

	query.bind(year);

	auto set = query.execute();
	if (set.hasData())
	{
		auto rows = set.fetchAll();
		for (auto row : rows)
		{
			std::cout << "transactions avarege: " << row.get(0) << ", " << "Month: " <<row.get(1) << ", " << "year: " << row.get(2) << endl;
		}

	}

	else
	{
		std::cout << "there are no transactions at:" << year << " " << "try differnt year" << endl;
	}


 }







//query 25, employee who sold the most books in a given month
void bestSellingEmployee(Session& sess, int month, int year)
{
	auto query = sess.sql(R"(
	SELECT COUNT(employee_id) AS numOfSales, purch.employee_id, emp.firstName, emp.lastName FROM bookstore.purchase AS purch JOIN employees AS emp ON purch.employee_id = emp.idEmployees where month(purchasDate) = ? and year(purchasDate) = ? GROUP BY employee_id ORDER BY numOfSales DESC LIMIT 1;
	)");

	query.bind(month);
	query.bind(year);

	auto set = query.execute();
	auto row = set.fetchOne();

	if (row)
	{
		
		
			std::cout << row.get(2) << " " << row.get(3) << ", employee id: " << row.get(1) << ", has made " << row.get(0) << " during that month." << std::endl;
		
		std::cout << std::endl;
	}
	else
	{
		std::cout << "Could not find any sales made in " << month << " "<<year << std::endl;
	}
}



//query 9, purchase history of customer
void purchaseHistory(Session& sess, std::string CustomerFN, std::string customerLN)
{
	auto query = sess.sql(R"(  SELECT 
		customer.firstName,
		customer.lastName,
		DATE_FORMAT(purchase.purchasDate, "%Y-%M-%d"),
		books.bookName,
		store_purchase.price
		FROM
		bookstore.books_in_shipments
		INNER JOIN
		bookstore.shipments ON shipments.shipment_id = books_in_shipments.shipment_id
		INNER JOIN
		bookstore.purchase ON shipments.purchase_id = purchase.purchase_id
		INNER JOIN
		bookstore.customer ON purchase.customer_id = customer.customer_id
		INNER JOIN
		bookstore.store_purchase ON store_purchase.store_purchase_id = books_in_shipments.store_purchase_id
		inner join
		bookstore.books on store_purchase.book_id = books.book_id
		WHERE
		customer.firstName = ? and
		customer.lastName=?
		ORDER BY purchase.purchasDate ASC;)");

	query.bind(CustomerFN);
	query.bind(customerLN);
	auto set = query.execute();
	auto rows = set.fetchAll();
	if (set.hasData())
	{

		for (auto row : rows)
		{
			std::cout << "Customer:" << row.get(0) << "  " << row.get(1) << " , " << "purchase date:" << row.get(2) << ", " << "Book name: " << row.get(3) << " , " << "Price:" << row.get(4) << endl;
		}
	}
	else
	{
		std::cout << "this customer does not have a purchase history" << endl;
	}
}

//query 10, reservation history for a given customer
void reservationHistory(Session& sess, int customerId)
{
	auto query = sess.sql(R"(
	select cu.firstName, cu.lastName, b.bookName, res.reservationStatus, DATE_FORMAT(res.reservationDate,"%Y-%M-%d")
	from reservations as res 
	join books as b on res.book_id = b.book_id
	join customer as cu on res.customer_id = cu.customer_id
	where res.customer_id =?;
	)");

	query.bind(customerId);

	auto set = query.execute();
	auto rows = set.fetchAll();

	if (set.hasData())
	{
		
		for (auto row : rows)
		{
			std::cout << "Reservation history for " << row.get(0) << " " << row.get(1) << ": " << std::endl;
			std::cout << "book name: " << row.get(2) << ", reservation status: " << row.get(3) << " , reservation date: "<< row.get(4) << std::endl;
		}
		std::cout << std::endl;
	}
	else
	{
		std::cout << "Could not find reservations for customer id " << customerId  << "."<< std::endl;
	}
}



int main(int argc, const char* argv[])
{
	const char *url = (argc > 1 ? argv[1] : "mysqlx://mysqluser:mysqlpassword@178.79.166.104");
	cout << "Creating session on " << url
		 << " ..." << endl;
	Session sess(url);
	sess.sql("USE bookstore").execute();
	

	//cout << "ship status" << endl;
	//getShipmentStatus(sess, 3);

	//cout << "oldest book" << endl;
	//oldestBook(sess);

				 /*cout << "ship status" << endl;
	getShipmentStatus(sess, 3);

	//cout << "copies sold" << endl;
	//copiesSold(sess, "The Adventures of Sherlock Holmes");

	//cout << "ship status" << endl;
	//favAuthor(sess, "2020-04-05" ,"2020-07-30");

	//cout << "fave authoer:" << endl;
	//favAuthor(sess, "2000-1-1", "2020-1-1");

	//cout << "split shipments" << endl;
	//splitShipments(sess, "George", "Washington");

	//cout << "reservation list" << endl;
	//reservationList(sess);

	//cout << "abvAVG" << endl;
	//aboveAvg(sess);

	//cout << "ship status" << endl;
	//getShipmentStatus(sess, 3);

	//cout << "OLDEST BOOK :" << endl;
	//oldestBook(sess);

	//cout << "IS BOOK IN STOCK :" << endl;
	//isBookInStock(sess, "Animal Farm");

	//cout << "OLDEST CUSTOMER" << endl;
	//whoIsTheOldestCustomer(sess);

	//cout << "RESERVATION LIST" << endl;
	//reservationList(sess);

	//cout << "TOP 3 CUSTOMER" << endl;
	//top3Customers(sess);

	//cout << "MOST TRANSLATED BOOK :" << endl;
	//mostTranslatedBook(sess);
	//cout << "CLAC SHIPMENT" << endl;
	//calculateShipping(sess, 1);


	diffEdition(sess);
	cout << "MOST TRANSLATED BOOK :" << endl;
	mostTranslatedBook(sess);
	cout << "CLAC SHIPMENT" << endl;
	calculateShipping(sess, 1);*/

	//sumXpress(sess, 6, 2020);

	//sumBit(sess, 6, 2020);

	//numOfShippingsExpressAndPost(sess);
	//diffEdition(sess);
	//sumBit(sess, 6, 2020);
	//noPurchaseInThePastTwoYears(sess);

	//haventPurchasedReservations(sess);
	//storePurchaseBetweenTwoDates(sess, "2000-07-07", "2020-07-07");
	//calculateSalary(sess, 12, 7, 2020);
	//monthlyStorage(sess, 2019);
	//bestSellingEmployee(sess, 6, 2020);
	//reservationHistory(sess, 1);

	//mostTranslatedBook(sess);
	//calculateSalary(sess, 12, 7, 2020);
	//monthlyStorage(sess, 2019);
	/*bestSellingEmployee(sess, 6, 2020);*/
	/*monthProfit(sess, 2019, 5);*/
	/*avgTrans(sess, 1980);*/
	/*getShipmentStatus(sess, 500);*/
	//calculateSalary(sess, 1, 5, 2019);
	/*reservationList(sess);*/
	//purchaseHistory(sess, "George", "Washington");
	/*monthlyStorage(sess, 2024);*/
	/*reservationList(sess);*/
	top3Customers(sess);


	sess.close();
}
